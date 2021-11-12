#pragma once

#include "envoy/extensions/filters/http/lua/v3/ubpf.pb.h"
#include "envoy/extensions/filters/http/lua/v3/ubpf.pb.validate.h"

#include "source/extensions/filters/http/common/factory_base.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Ubpf {

/**
 * Config registration for the uBPF filter. @see NamedHttpFilterConfigFactory.
 */
class UbpfFilterConfig
    : public Common::FactoryBase<envoy::extensions::filters::http::ubpf::v3::Ubpf,
                                 envoy::extensions::filters::http::ubpf::v3::UbpfPerRoute> {
public:
  LuaFilterConfig() : FactoryBase("envoy.filters.http.lua") {}

private:
  Http::FilterFactoryCb createFilterFactoryFromProtoTyped(
      const envoy::extensions::filters::http::lua::v3::Lua& proto_config, const std::string&,
      Server::Configuration::FactoryContext& context) override;

  Router::RouteSpecificFilterConfigConstSharedPtr createRouteSpecificFilterConfigTyped(
      const envoy::extensions::filters::http::lua::v3::LuaPerRoute& proto_config,
      Server::Configuration::ServerFactoryContext& context,
      ProtobufMessage::ValidationVisitor& validator) override;
};

} // namespace Lua
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
