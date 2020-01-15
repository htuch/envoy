#include "test/common/stats/stat_test_utility.h"

#include "envoy/config/cluster/v3alpha/cluster.pb.h"

#include "gtest/gtest.h"

namespace Envoy {
namespace {

TEST(HostOverheadTest, All) { 
  const uint32_t HostCount = 100000;
  Stats::TestUtil::MemoryTest memory_test;
  envoy::config::cluster::v3alpha::Cluster cluster;
  ENVOY_LOG_MISC(debug, "HTD {}", memory_test.consumedBytes());
  for (uint32_t i=0; i < HostCount; ++i) {
    cluster.add_hosts();
  }
  ENVOY_LOG_MISC(debug, "HTD {}", memory_test.consumedBytes());
}

} // namespace
} // namespace Envoy
