#pragma once

#include <chrono>

#include "envoy/common/optional.h"
#include "envoy/common/pure.h"
#include "envoy/http/header_map.h"

namespace Envoy {
namespace Grpc {

/**
 * Async Client failure reasons.
 */
enum class AsyncClientFailureReason {
  // The stream has been reset.
  Reset
};

template<class RequestType>
class AsyncClientStream {
public:
  virtual ~AsyncClientStream() {}

  /**
   * Send request message to the stream.
   * @param request protobuf serializable message.
   * @param end_stream supplies whether this is the last message.
   */
  virtual void sendMessage(const RequestType& request, bool end_stream) PURE;

  /***
   * Reset the stream.
   */
  virtual void reset() PURE;
};

/**
 * Notifies caller of async gRPC stream status.
 */
template<class ResponseType>
class AsyncClientCallbacks {
public:
  virtual ~AsyncClientCallbacks() {}

  /**
   * Called when initial metadata is received on the stream.
   * @param metadata the initial metadata.
   */
  virtual void onInitialMetadata(const Http::HeaderMap& metadata) PURE;

  /**
   * Called when an async gRPC message is received.
   * @param response the gRPC message.
   */
  virtual void onResponseMessage(const ResponseType& message) PURE;

  /**
   * Called when an async gRPC stream is remotely closed with successful status.
   * @param metadata the trailing metadata.
   */
  virtual void onCompletion(const Http::HeaderMap& metadata) PURE;

  /**
   * Called when the async gRPC stream fails.
   * @param reason the gRPC FailureReason.
   */
  virtual void onFailure(AsyncClientFailureReason reason) PURE;
};

/**
 * Supports sending gRPC requests and receiving responses asynchronously. This can be used to
 * implement either plain RPC or streaming gRPC calls.
 */
template<class RequestType, class ResponseType>
class AsyncClient {
public:
  virtual ~AsyncClient() {}

  /**
   * Start a gRPC stream asynchronously.
   * @param service_full_name gRPC service name.
   * @param method_name gRPC method name.
   * @param metadata optional initial metadata.
   * @param end_stream supplies whether this is a headers-only request.
   * @param callbacks the callbacks to be notified of stream status.
   * @param timeout supplies the stream timeout, measured since when the frame with end_stream
   *        flag is sent until when the first frame is received.
   * @return a stream handle or nullptr if no stream could be started. NOTE: In this case
   *         onFailure() has already been called inline. The client owns the stream and
   *         the handle can be used to send more messages or close the stream.
   */
  virtual AsyncClientStream<RequestType>*
  start(const std::string& service_full_name, const std::string& method_name,
        Optional<const Http::HeaderMap&> metadata, bool end_stream,
        AsyncClientCallbacks<ResponseType>& callbacks,
        const Optional<std::chrono::milliseconds>& timeout) PURE;
};

} // Grpc
} // Envoy
