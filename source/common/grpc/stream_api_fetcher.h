#pragma once

#include <chrono>
#include <string>

#include "envoy/event/dispatcher.h"
#include "envoy/runtime/runtime.h"
#include "envoy/upstream/cluster_manager.h"

namespace Envoy {
namespace Grpc {

/**
 * A helper base class used to subscribe to a gRPC stream (or the equivalent transcoded REST API
 * polled at a jittered periodic interval). Once initialize() is called, the API will be fetched and
 * events raised. This is similar to RestApiFetcher, but also supports gRPC streaming and operates
 * on gRPC delimited messages. gRPC stream failures result in attempts to reconnect using the given
 * refresh interval.
 */
template <class RequestType, class ResponseType>
class StreamApiFetcher : public Http::AsyncClient::Callbacks {
protected:
  StreamApiFetcher(Upstream::ClusterManager& cm, const std::string& remote_cluster_name,
                   bool rest_transcoded, Event::Dispatcher& dispatcher,
                   Runtime::RandomGenerator& random, std::chrono::milliseconds refresh_interval);
  ~StreamApiFetcher();

  /**
   * Start the stream fetch sequence. This should be called once.
   */
  void initialize();


  /**
   * This will be called when a new stream is about to be created. It should be overridden to fill
   * the request message with a valid request. When rest_transcoded is set to false, a gRPC stream
   * is created and parseReponse() will be invoked on every received delimited message. Otherwise, a
   * REST POST is performed and parseReponse() is invoked on a 200 response.
   */
  virtual void createStream(RequestType& request) PURE;

  /**
   * This will be called when a delimited gRPC message is received or a 200 response is returned by
   * the REST API with the response message.
   */
  virtual void parseMessage(const ResponseType& response) PURE;

  /**
   * This will be called either in the success case or in the failure case for each underlying HTTP
   * stream. It can be used to hold common post request logic. It will be invoked prior to the next
   * createRequest().
   */
  virtual void onStreamComplete() PURE;

  /**
   * This will be called if the stream fails (either due to non-200 response, network error, etc.).
   * @param e supplies any exception data on why the fetch failed. May be nullptr.
   */
  virtual void onStreamFailure(EnvoyException* e) PURE;

protected:
  const std::string remote_cluster_name_;
  Upstream::ClusterManager& cm_;

private:
  void refresh();
  void requestComplete();

  // Http::AsyncClient::Callbacks
  void onSuccess(Http::MessagePtr&& response) override;
  void onFailure(Http::AsyncClient::FailureReason reason) override;

  Runtime::RandomGenerator& random_;
  const std::chrono::milliseconds refresh_interval_;
  Event::TimerPtr refresh_timer_;
  Http::AsyncClient::Request* active_request_{};
  bool rest_transcoded_;
};

} // Grpc
} // Envoy
