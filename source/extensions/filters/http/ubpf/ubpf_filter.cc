#include "source/extensions/filters/http/ubpf/ubpf_filter.h"

#include "source/common/common/logger.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Ubpf {

Filter::Filter() {
  ENVOY_LOG_MISC(debug, "ubpf: init");
}

} // namespace Ubpf
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
