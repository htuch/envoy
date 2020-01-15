#include "common/memory/stats.h"

#include "gtest/gtest.h"

namespace Envoy {
namespace {

TEST_F(HostOverheadTest, All) { 
  Stats::TestUtil::MemoryTest memory_test;
  ENVOY_LOG_MISC(debug, "HTD {}", memory_test.consumedBytes());
}

} // namespace
} // namespace Envoy
