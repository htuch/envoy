#pragma once

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Ubpf {

class Filter : public Http::StreamFilter {
public:
  Filter();

  Http::FilterHeadersStatus decodeHeaders(Http::RequestHeaderMap& headers,
                                          bool end_stream) override {
  }
}

} // namespace Ubpf
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
