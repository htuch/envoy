#include "common/grpc/async_client_impl.h"

#include "common/http/header_map_impl.h"
#include "common/grpc/common.h"

namespace Envoy {
namespace Grpc {

template <class RequestType, class ResponseType>
AsyncClientImpl<RequestType, ResponseType>::AsyncClientImpl(Upstream::ClusterManager& cm,
                                                            const std::string& remote_cluster_name)
    : cm_(cm), remote_cluster_name_(remote_cluster_name) {}

template <class RequestType, class ResponseType>
typename AsyncClientStream<RequestType>::Stream* AsyncClientImpl<RequestType, ResponseType>::start(
    const std::string& service_full_name, const std::string& method_name,
    Optional<const Http::HeaderMap&> metadata, bool end_stream,
    AsyncClientCallbacks<ResponseType>& callbacks,
    const Optional<std::chrono::milliseconds>& timeout) {
  std::unique_ptr<AsyncClientStreamImpl<RequestType, ResponseType>> grpc_stream{
      new AsyncClientStreamImpl<RequestType, ResponseType>(*this, callbacks)};
  Http::AsyncClient::Stream* http_stream =
      cm_.httpAsyncClientForCluster(remote_cluster_name_)
          .start(*grpc_stream, Optional<std::chrono::milliseconds>(timeout));
  grpc_stream->set_http_stream(http_stream);

  if (http_stream == nullptr) {
    callbacks.onFailure(AsyncClientFailureReason::Reset);
    return nullptr;
  }

  Http::MessagePtr message =
      Common::prepareHeaders(remote_cluster_name_, service_full_name, method_name);

  // TODO(htuch): Fill in initial metadata in headers.
  http_stream->sendHeaders(message->headers(), end_stream);

  grpc_stream->moveIntoList(std::move(grpc_stream), active_streams_);
  return active_streams_.front().get();
}

template <class RequestType, class ResponseType>
AsyncClientStreamImpl<RequestType, ResponseType>::AsyncClientStreamImpl(
    AsyncClientImpl<RequestType, ResponseType>& parent,
    AsyncClientCallbacks<ResponseType>& callbacks)
    : parent_(parent), callbacks_(callbacks) {}

template <class RequestType, class ResponseType>
void AsyncClientStreamImpl<RequestType, ResponseType>::onHeaders(Http::HeaderMapPtr&& headers,
                                                                 bool end_stream) {
  try {
    // TODO(htuch): support initial metadata calllback
    if (end_stream) {
      Common::validateTrailers(*headers);
    } else {
      Common::validateHeaders(*headers);
    }
  } catch (const Exception& e) {
    // TODO(htuch): implement onFailure handling
    NOT_IMPLEMENTED;
  }
}

template <class RequestType, class ResponseType>
void AsyncClientStreamImpl<RequestType, ResponseType>::onData(Buffer::Instance& data,
                                                              bool end_stream) {
  std::vector<Frame> frames;
  if (!decoder_.decode(data, frames)) {
    // TODO(htuch): handle decoder failures.
    NOT_IMPLEMENTED;
    return;
  }

  for (const auto& frame : frames) {
    ResponseType response;
    // TODO(htuch): check frame.flags_ == GRPC_FH_DEFAULT
    if (!response.ParseFromArray(frame.data_->linearize(frame.data_->length()),
                                 frame.data_->length())) {
      // TODO(htuch): handle failures
      continue;
    }
    callbacks_.onResponseMessage(response);
  }
}

template <class RequestType, class ResponseType>
void AsyncClientStreamImpl<RequestType, ResponseType>::onTrailers(Http::HeaderMapPtr&& trailers) {
  try {
    Common::validateTrailers(*trailers);
    // TODO(htuch): separate out trailing metadata
    callbacks_.onCompletion(*trailers);
  } catch (const Exception& e) {
    // TODO(htuch): handle failures
    NOT_IMPLEMENTED;
  }
}

template <class RequestType, class ResponseType>
void AsyncClientStreamImpl<RequestType, ResponseType>::onReset() {
  // TODO(htuch):
  NOT_IMPLEMENTED;
}

template <class RequestType, class ResponseType>
void AsyncClientStreamImpl<RequestType, ResponseType>::sendMessage(const RequestType& request,
                                                                   bool end_stream) {
  stream_->sendData(*Common::serializeBody(request), end_stream);
}

template <class RequestType, class ResponseType>
void AsyncClientStreamImpl<RequestType, ResponseType>::reset() {
  stream_->reset(); 
  cleanup();
}

} // Http
} // Envoy
