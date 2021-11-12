#pragma once

#include "envoy/extensions/filters/http/ubpf/v3/ubpf.pb.h"
#include "envoy/extensions/filters/http/ubpf/v3/ubpf.pb.validate.h"

#include "source/extensions/filters/http/common/factory_base.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Ubpf {

/**
 * Config registration for the uBPF filter. @see NamedHttpFilterConfigFactory.
 */
class UbpfFilterConfig
    : public Common::FactoryBase<envoy::extensions::filters::http::ubpf::v3::Ubpf> {
public:
  UbpfFilterConfig() : FactoryBase("envoy.filters.http.ubpf") {}

private:
  Http::FilterFactoryCb createFilterFactoryFromProtoTyped(
      const envoy::extensions::filters::http::ubpf::v3::Ubpf& proto_config, const std::string&,
      Server::Configuration::FactoryContext& context) override;
};

} // namespace Ubpf
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
