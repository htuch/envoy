#include "source/extensions/filters/http/ubpf/config.h"

#include "envoy/registry/registry.h"

#include "source/extensions/filters/http/ubpf/ubpf_filter.h"

#include "ubpf.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Ubpf {

Http::FilterFactoryCb UbpfFilterConfig::createFilterFactoryFromProtoTyped(
    const envoy::extensions::filters::http::ubpf::v3::Ubpf& proto_config, const std::string&,
    Server::Configuration::FactoryContext& context) {
  return [path = proto_config.bpf_code().filename(),
          &context](Http::FilterChainFactoryCallbacks& callbacks) -> void {
    callbacks.addStreamDecoderFilter(std::make_shared<Filter>(context.api(), path));
  };
}

/**
 * Static registration for the Ubpf filter. @see RegisterFactory.
 */
REGISTER_FACTORY(UbpfFilterConfig,
                 Server::Configuration::NamedHttpFilterConfigFactory){"envoy.ubpf"};

} // namespace Ubpf
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
