#include "common/config/api_type_oracle.h"

#include "envoy/common/exception.h"

#include "common/common/assert.h"
#include "common/common/logger.h"

#include "absl/container/flat_hash_map.h"
#include "udpa/annotations/versioning.pb.h"
#include "udpa/type/v1/typed_struct.pb.h"

namespace Envoy {
namespace Config {

namespace {

using V2ApiTypeMap = absl::flat_hash_map<std::string, std::string>;

const V2ApiTypeMap& v2ApiTypeMap() {
  CONSTRUCT_ON_FIRST_USE(V2ApiTypeMap,
                         {"envoy.ip_tagging", "envoy.config.filter.http.ip_tagging.v2.IPTagging"});
}

// TODO(htuch) this is TypeUtil::typeUrlToDescriptorFullName inlined for dep
// cycle reasons, refactor stuff
absl::string_view typeUrlToDescriptorFullName(absl::string_view type_url) {
  const size_t pos = type_url.rfind('/');
  if (pos != absl::string_view::npos) {
    type_url = type_url.substr(pos + 1);
  }
  return type_url;
}

} // namespace

const Protobuf::Descriptor*
ApiTypeOracle::inferEarlierVersionDescriptor(absl::string_view extension_name,
                                             const ProtobufWkt::Any& typed_config,
                                             absl::string_view target_type) {
  ENVOY_LOG_MISC(trace, "Inferring earlier type for {} (extension {})", target_type,
                 extension_name);
  // Determine what the type of configuration implied by typed_config is.
  absl::string_view type = typeUrlToDescriptorFullName(typed_config.type_url());
  udpa::type::v1::TypedStruct typed_struct;
  if (type == udpa::type::v1::TypedStruct::default_instance().GetDescriptor()->full_name()) {
    if (!typed_config.UnpackTo(&typed_struct)) {
      throw EnvoyException(fmt::format("Unable to unpack as {}: {}",
                                       typed_struct.GetDescriptor()->full_name(),
                                       typed_config.DebugString()));
    }
    type = typeUrlToDescriptorFullName(typed_struct.type_url());
    ENVOY_LOG_MISC(trace, "Extracted embedded type {}", type);
  }

  // If we can't find an explicit type, this is likely v2, so we need to consult
  // a static map.
  if (type.empty()) {
    auto it = v2ApiTypeMap().find(extension_name);
    if (it == v2ApiTypeMap().end()) {
      ENVOY_LOG_MISC(trace, "Missing v2 API type map");
      // return nullptr;
    } else {
      type = it->second;
    }
  }

  // Determine if there is an earlier API version for target_type.
  std::string previous_target_type;
  const Protobuf::Descriptor* desc =
      Protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(std::string{target_type});
  if (desc == nullptr) {
    ENVOY_LOG_MISC(trace, "No descriptor found for {}", target_type);
    return nullptr;
  }
  if (desc->options().HasExtension(udpa::annotations::versioning)) {
    previous_target_type =
        desc->options().GetExtension(udpa::annotations::versioning).previous_message_type();
  }

  if (!previous_target_type.empty() && type != target_type) {
    const Protobuf::Descriptor* desc =
        Protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(previous_target_type);
    ASSERT(desc != nullptr);
    ENVOY_LOG_MISC(trace, "Inferred {}", desc->full_name());
    return desc;
  }

  return nullptr;
}

} // namespace Config
} // namespace Envoy
