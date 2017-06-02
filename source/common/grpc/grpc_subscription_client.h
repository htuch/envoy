#pragma once

#include "envoy/http/async_client.h"
#include "envoy/http/subscription_api_client.h"
#include "envoy/upstream/cluster_manager.h"

#include "common/grpc/codec.h"

namespace Envoy {
namespace Grpc {

template <class RequestType, class ResponseType>
class GrpcSubscriptionClient : public Http::SubscriptionApiClient<RequestType, ResponseType>,
                               Http::AsyncClient::StreamCallbacks {
public:
  GrpcSubscriptionClient(Upstream::ClusterManager& cm, const std::string& remote_cluster_name,
                         const std::string& service_full_name, const std::string& method_name);
  ~GrpcSubscriptionClient() override;

  // Http::SubscriptionApiClient
  void start(const RequestType& request,
             Http::SubscriptionApiCallbacks<ResponseType>& callbacks) override;

  // Http::AsyncClient::StreamCallbacks
  void onHeaders(Http::HeaderMapPtr&& headers, bool end_stream) override;
  void onData(Buffer::Instance& data, bool end_stream) override;
  void onTrailers(Http::HeaderMapPtr&& trailers) override;
  void onReset() override;

private:
  Upstream::ClusterManager& cm_;
  const std::string remote_cluster_name_;
  const std::string service_full_name_;
  const std::string method_name_;
  Http::AsyncClient::Stream* stream_{};
  Http::SubscriptionApiCallbacks<ResponseType> callbacks_;
  Decoder decoder_;
};

} // Http
} // Grpc
