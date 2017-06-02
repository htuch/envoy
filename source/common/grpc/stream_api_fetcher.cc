#include "common/grpc/stream_api_fetcher.h"

#include <chrono>
#include <cstdint>
#include <string>

#include "common/common/enum_to_int.h"
#include "common/http/message_impl.h"
#include "common/http/utility.h"

namespace Envoy {
namespace Grpc {

template <class RequestType, class ResponseType>
StreamApiFetcher<RequestType, ResponseType>::StreamApiFetcher(
    Upstream::ClusterManager& cm, const std::string& remote_cluster_name, bool rest_transcoded,
    const std::string& path,
    Event::Dispatcher& dispatcher, Runtime::RandomGenerator& random,
    std::chrono::milliseconds refresh_interval)
    : remote_cluster_name_(remote_cluster_name), cm_(cm), random_(random),
      refresh_interval_(refresh_interval),
      refresh_timer_(dispatcher.createTimer([this]() -> void { refresh(); })),
      rest_transcoded_(rest_transcoded) {}

template <class RequestType, class ResponseType>
StreamApiFetcher<RequestType, ResponseType>::~StreamApiFetcher() {
  if (active_request_) {
    active_request_->cancel();
  }
}

template <class RequestType, class ResponseType>
void StreamApiFetcher<RequestType, ResponseType>::initialize() {
  refresh();
}

template <class RequestType, class ResponseType>
void StreamApiFetcher<RequestType, ResponseType>::onSuccess(Http::MessagePtr&& response) {
  uint64_t response_code = Http::Utility::getResponseStatus(response->headers());
  if (response_code != enumToInt(Http::Code::OK)) {
    onFailure(Http::AsyncClient::FailureReason::Reset);
    return;
  }

  try {
    parseMessage(*response);
  } catch (EnvoyException& e) {
    onStreamFailure(&e);
  }

  requestComplete();
}

template <class RequestType, class ResponseType>
void StreamApiFetcher<RequestType, ResponseType>::onFailure(Http::AsyncClient::FailureReason) {
  onStreamFailure(nullptr);
  requestComplete();
}

template <class RequestType, class ResponseType>
void StreamApiFetcher<RequestType, ResponseType>::refresh() {
  Http::MessagePtr message(new Http::RequestMessageImpl());
  createStream(*message);
  message->headers().insertHost().value(remote_cluster_name_);
  active_request_ = cm_.httpAsyncClientForCluster(remote_cluster_name_)
                        .send(std::move(message), *this,
                              Optional<std::chrono::milliseconds>(std::chrono::milliseconds(1000)));
}

template <class RequestType, class ResponseType>
void StreamApiFetcher<RequestType, ResponseType>::requestComplete() {
  onStreamComplete();
  active_request_ = nullptr;

  // Add refresh jitter based on the configured interval.
  std::chrono::milliseconds final_delay =
      refresh_interval_ + std::chrono::milliseconds(random_.random() % refresh_interval_.count());

  refresh_timer_->enableTimer(final_delay);
}

} // Grpc
} // Envoy
