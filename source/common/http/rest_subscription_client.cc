#include "common/http/rest_subscription_client.h"

#include "common/http/headers.h"

#include <google/protobuf/util/json_util.h>

namespace Envoy {
namespace Http {

template <class RequestType, class ResponseType>
RestSubscriptionClient<RequestType, ResponseType>::RestSubscriptionClient(
    Upstream::ClusterManager& cm, const std::string& remote_cluster_name, const std::string& path,
    Event::Dispatcher& dispatcher, Runtime::RandomGenerator& random,
    std::chrono::milliseconds refresh_interval)
    : RestApiFetcher(cm, remote_cluster_name, dispatcher, random, refresh_interval), path_(path) {}

template <class RequestType, class ResponseType>
void RestSubscriptionClient<RequestType, ResponseType>::createRequest(Message& message) {
  message.headers().insertMethod().value(Http::Headers::get().MethodValues.Post);
  message.headers().insertPath().value(path_);
  message.body()->add(request_json_);
}

template <class RequestType, class ResponseType>
void RestSubscriptionClient<RequestType, ResponseType>::parseResponse(const Message& message) {
  ResponseType response;
  // FIXME: check status
  google::protobuf::util::JsonStringToMessage(response.bodyAsString(), response);
  callbacks_->onResponse(response);
}

template <class RequestType, class ResponseType>
void RestSubscriptionClient<RequestType, ResponseType>::onFetchComplete() {}

template <class RequestType, class ResponseType>
void RestSubscriptionClient<RequestType, ResponseType>::onFetchFailure(EnvoyException* e) {}

template <class RequestType, class ResponseType>
void RestSubscriptionClient<RequestType, ResponseType>::start(const RequestType& request,
           SubscriptionApiCallbacks<ResponseType>& callbacks) {
  google::protobuf::util::JsonOptions json_options;
  // FIXME: check status
  google::protobuf::util::MessageToJsonString(request, request_json_, json_options);
  callbacks_ = callbacks;
}

} // Http
} // Envoy
