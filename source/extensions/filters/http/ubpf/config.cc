#include "ubpf.h"
#include "source/extensions/filters/http/ubpf/config.h"

#include "envoy/registry/registry.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Ubpf {

Http::FilterFactoryCb UbpfFilterConfig::createFilterFactoryFromProtoTyped(
    const envoy::extensions::filters::http::ubpf::v3::Ubpf& proto_config, const std::string&,
    Server::Configuration::FactoryContext& context) {
  FilterConfigConstSharedPtr filter_config(new FilterConfig{
      proto_config, context.threadLocal(), context.clusterManager(), context.api()});
  auto& time_source = context.mainThreadDispatcher().timeSource();
  return [filter_config, &time_source](Http::FilterChainFactoryCallbacks& callbacks) -> void {
    callbacks.addStreamFilter(std::make_shared<Filter>(filter_config, time_source));
  };
}

/**
 * Static registration for the Ubpf filter. @see RegisterFactory.
 */
REGISTER_FACTORY(UbpfFilterConfig, Server::Configuration::NamedHttpFilterConfigFactory){"envoy.ubpf"};

} // namespace Ubpf
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
