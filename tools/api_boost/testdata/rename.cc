#include "envoy/api/v2/route/route.pb.h"

using envoy::api::v2::route::RouteAction;
using MutableRouteActionAccessor = std::string* (RouteAction::*)();

void test() {
  envoy::api::v2::route::RouteAction route_action;
  route_action.host_rewrite();
  route_action.set_host_rewrite("blah");
  MutableRouteActionAccessor foo = &envoy::api::v2::route::RouteAction::mutable_host_rewrite;
}
