#pragma once

#include "envoy/common/pure.h"

namespace Envoy {
namespace Http {

template <class ResponseType> class SubscriptionApiCallbacks {
public:
  virtual ~SubscriptionApiCallbacks() {}

  virtual void onResponse(const ResponseType& response) PURE;
};

/**
 * Common abstraction for clients (e.g. REST, gRPC) that operate on a subscription-like API.
 * Here we consider a subscription API a server streaming channel either implemented with something
 * like gRPC server streaming RPCs or with consecutive REST API calls (at some periodic interval or
 * long polling).
 */
template<class RequestType, class ResponseType>
class SubscriptionApiClient {
public:
  virtual ~SubscriptionApiClient() {}

  /**
   * Start an API subscription asynchronously. This should be called once.
   * @param request a protobuffer serializable value of RequestType.
   * @param callbacks the callbacks to be notified of stream responses and status.
   * @return a stream handle or nullptr if no stream could be started. NOTE: In this case
   *         onEndOfStream() has already been called inline. The client owns the stream and
   *         the handle can be used to send more messages or close the stream.
   */
  virtual void start(const RequestType& request,
                     SubscriptionApiCallbacks<ResponseType>& callbacks) PURE;
};

} // Http
} // Envoy
