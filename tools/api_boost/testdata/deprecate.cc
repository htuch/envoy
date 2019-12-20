#include "envoy/api/v2/route/route.pb.h"

void test() {
  envoy::api::v2::route::VirtualHost vhost;
  vhost.per_filter_config();
  vhost.mutable_per_filter_config();
}
