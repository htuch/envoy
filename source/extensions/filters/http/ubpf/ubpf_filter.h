#pragma once

#include "envoy/http/filter.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Ubpf {

class Filter : public Http::StreamDecoderFilter {
public:
  Filter(Api::Api& api);

  void onDestroy() override {}

  Http::FilterHeadersStatus decodeHeaders(Http::RequestHeaderMap& /*headers*/,
                                          bool /*end_stream*/) override {
    return Http::FilterHeadersStatus::Continue;
  }

  Http::FilterDataStatus decodeData(Buffer::Instance& /*data*/, bool /*end_stream*/) override {
    return Http::FilterDataStatus::Continue;
  }
  Http::FilterTrailersStatus decodeTrailers(Http::RequestTrailerMap& /*trailers*/) override {
    return Http::FilterTrailersStatus::Continue;
  }
  void setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& /*callbacks*/) override {
  }
};

} // namespace Ubpf
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
