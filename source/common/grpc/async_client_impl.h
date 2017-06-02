#pragma once

#include "envoy/grpc/async_client.h"

#include "common/grpc/codec.h"
#include "common/http/async_client_impl.h"

namespace Envoy {
namespace Grpc {

template<class RequestType, class ResponseType>
class AsyncClientStreamImpl;

template<class RequestType, class ResponseType>
class AsyncClientImpl final : public AsyncClient<RequestType, ResponseType>, Http::AsyncClient::StreamCallbacks {
public:
  AsyncClientImpl(Upstream::ClusterManager& cm, const std::string& remote_cluster_name);
  ~AsyncClientImpl() override {
    // TODO(htuch): cleanup active_streams_ as per Http::AsyncClient;
  }

  // Grpc::AsyncClient
  typename AsyncClientStream<RequestType>::Stream*
  start(const std::string& service_full_name, const std::string& method_name,
        Optional<const Http::HeaderMap&> metadata, bool end_stream,
        AsyncClientCallbacks<ResponseType>& callbacks,
        const Optional<std::chrono::milliseconds>& timeout) override;

private:
  Upstream::ClusterManager& cm_;
  const std::string remote_cluster_name_;
  std::list<std::unique_ptr<AsyncClientStreamImpl<RequestType, ResponseType>>> active_streams_;
};

template<class RequestType, class ResponseType>
class AsyncClientStreamImpl final : public AsyncClientStream<RequestType> {
public:
  AsyncClientStreamImpl(AsyncClientImpl<RequestType, ResponseType>& parent,
                        AsyncClientCallbacks<ResponseType>& callbacks);

  // Http::AsyncClient::StreamCallbacks
  void onHeaders(Http::HeaderMapPtr&& headers, bool end_stream) override;
  void onData(Buffer::Instance& data, bool end_stream) override;
  void onTrailers(Http::HeaderMapPtr&& trailers) override;
  void onReset() override;

  // Grpc::AsyncClientStream
  void sendMessage(const RequestType& request, bool end_stream) override;
  void reset() override;

  void set_http_stream(Http::AsyncClient::Stream* stream) { stream_ = stream; }

private:
  AsyncClientImpl<RequestType, ResponseType>& parent_;
  AsyncClientCallbacks<ResponseType>& callbacks_;
  Http::AsyncClient::Stream* stream_{};
  Decoder decoder_;
};

} // Http
} // Envoy
