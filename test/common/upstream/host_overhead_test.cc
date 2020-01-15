#include "test/common/stats/stat_test_utility.h"

#include "envoy/config/cluster/v3alpha/cluster.pb.h"

#include "gtest/gtest.h"

namespace Envoy {
namespace {

using envoy::config::cluster::v3alpha::Cluster;

const uint32_t HostCount = 100000;

double measurePerHostOverhead(std::function<void(Cluster&)> mutate_fn) {
  Cluster cluster;
  Stats::TestUtil::MemoryTest memory_test;
  const size_t start_bytes = memory_test.consumedBytes();
  for (uint32_t i=0; i < HostCount; ++i) {
    mutate_fn(cluster);
  }
  const size_t consumed_bytes = memory_test.consumedBytes() - start_bytes;
  return (1.0 * consumed_bytes) / HostCount;
}

TEST(HostOverheadTest, All) { 
  const double hosts_overhead = measurePerHostOverhead([](Cluster& cluster) {
    cluster.add_hosts();
  });
  const double load_assignment_overhead = measurePerHostOverhead([](Cluster& cluster) {
    if (cluster.load_assignment().endpoints().empty()) {
      cluster.mutable_load_assignment()->add_endpoints();
    }
    cluster.mutable_load_assignment()->mutable_endpoints(0)->add_lb_endpoints();
  });
  ENVOY_LOG_MISC(debug, "hosts overhead per host: {}", hosts_overhead);
  ENVOY_LOG_MISC(debug, "load_assignment overhead per host: {}", load_assignment_overhead);
}

} // namespace
} // namespace Envoy
