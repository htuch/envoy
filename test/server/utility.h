#pragma once

#include <string>

#include "envoy/api/v3alpha/lds.pb.h"

#include "common/protobuf/utility.h"

#include "test/test_common/utility.h"

namespace Envoy {
namespace Server {
namespace {

inline envoy::api::v3alpha::Listener parseListenerFromV2Yaml(const std::string& yaml) {
  envoy::api::v3alpha::Listener listener;
  TestUtility::loadFromYaml(yaml, listener);
  return listener;
}

} // namespace
} // namespace Server
} // namespace Envoy
