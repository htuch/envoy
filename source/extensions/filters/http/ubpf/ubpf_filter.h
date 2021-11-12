#pragma once

#include "envoy/http/filter.h"

#include "ubpf.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Ubpf {

class Filter : public Http::StreamDecoderFilter {
public:
  Filter(Api::Api& api, const std::string& path);

  void onDestroy() override;
  Http::FilterHeadersStatus decodeHeaders(Http::RequestHeaderMap& headers,
                                          bool end_stream) override;

  Http::FilterDataStatus decodeData(Buffer::Instance& /*data*/, bool /*end_stream*/) override {
    return Http::FilterDataStatus::Continue;
  }
  Http::FilterTrailersStatus decodeTrailers(Http::RequestTrailerMap& /*trailers*/) override {
    return Http::FilterTrailersStatus::Continue;
  }
  void setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& /*callbacks*/) override {
  }

private:
  ubpf_vm* vm_{};
};

} // namespace Ubpf
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
