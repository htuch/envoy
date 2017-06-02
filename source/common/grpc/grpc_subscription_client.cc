#include "common/grpc/grpc_subscription_client.h"

#include "common/http/header_map_impl.h"
#include "common/grpc/common.h"

namespace Envoy {
namespace Grpc {

template <class RequestType, class ResponseType>
GrpcSubscriptionClient<RequestType, ResponseType>::GrpcSubscriptionClient(
    Upstream::ClusterManager& cm, const std::string& remote_cluster_name,
    const std::string& service_full_name, const std::string& method_name)
    : cm_(cm), remote_cluster_name_(remote_cluster_name), service_full_name_(service_full_name),
      method_name_(method_name) {}

template <class RequestType, class ResponseType>
GrpcSubscriptionClient<RequestType, ResponseType>::~GrpcSubscriptionClient() {
  if (stream_ != nullptr) {
    stream_->reset();
  }
}

template <class RequestType, class ResponseType>
void GrpcSubscriptionClient<RequestType, ResponseType>::start(
    const RequestType& request, Http::SubscriptionApiCallbacks<ResponseType>& callbacks) {
  stream_ = cm_.httpAsyncClientForCluster(remote_cluster_name_)
                .start(*this, Optional<std::chrono::milliseconds>(std::chrono::milliseconds(1000)));
  Http::MessagePtr message =
      Common::prepareHeaders(remote_cluster_name_, service_full_name_, method_name_);
  stream_->sendHeaders(message->headers(), false);
  stream_->sendData(*Common::serializeBody(request), true);
  callbacks_ = callbacks;
}

template <class RequestType, class ResponseType>
void GrpcSubscriptionClient<RequestType, ResponseType>::onHeaders(Http::HeaderMapPtr&& headers,
                                                                  bool end_stream) {
  try {
    if (end_stream) {
      Common::validateTrailers(*headers);
    } else {
      Common::validateHeaders(*headers);
    }
  } catch (const Exception& e) {
    // TODO(htuch): recovery from failed connects
    //onFailureWorker(e.grpc_status_, e.what());
  }
}

template <class RequestType, class ResponseType>
void GrpcSubscriptionClient<RequestType, ResponseType>::onData(Buffer::Instance& data,
                                                               bool end_stream) {
  std::vector<Frame> frames;
  if (!decoder_.decode(data, frames)) {
    // TODO(htuch): handle decoder failures.
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
    callbacks_.onResponse(response);
  }
}

template <class RequestType, class ResponseType>
void GrpcSubscriptionClient<RequestType, ResponseType>::onTrailers(Http::HeaderMapPtr&& trailers) {
  try {
    Common::validateTrailers(*trailers);
  } catch (const Exception& e) {
    // TODO(htuch): stats for failed streams, reconnect
    //onFailureWorker(e.grpc_status_, e.what());
  }
}

template <class RequestType, class ResponseType>
void GrpcSubscriptionClient<RequestType, ResponseType>::onReset() {
  // TODO(htuch): recovery from resets
}

} // Grpc
} // Envoy
