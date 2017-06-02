#pragma once

#include "envoy/http/subscription_api_client.h"

#include "common/http/rest_api_fetcher.h"

namespace Envoy {
namespace Http {

template <class RequestType, class ResponseType>
class RestSubscriptionClient : public RestApiFetcher,
                               SubscriptionApiClient<RequestType, ResponseType> {
public:
  RestSubscriptionClient(Upstream::ClusterManager& cm, const std::string& remote_cluster_name,
                         const std::string& path, Event::Dispatcher& dispatcher,
                         Runtime::RandomGenerator& random,
                         std::chrono::milliseconds refresh_interval);
  ~RestSubscriptionClient() override {}

  // RestApiFetcher
  void createRequest(Message& request) override;
  void parseResponse(const Message& response) override;
  void onFetchComplete() override;
  void onFetchFailure(EnvoyException* e) override;

  // SubscriptionApiClient
  void cancel() override;
  void start(const RequestType& request,
             SubscriptionApiCallbacks<ResponseType>& callbacks) override;

private:
  std::string request_json_;
  const std::string path_;
  SubscriptionApiCallbacks<ResponseType> callbacks_;
};

} // Http
} // Envoy
