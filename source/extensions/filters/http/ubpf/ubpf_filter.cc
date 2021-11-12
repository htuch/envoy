#include "source/extensions/filters/http/ubpf/ubpf_filter.h"

#include "source/common/common/logger.h"

#ifdef __cplusplus
extern "C"{
#endif 

#include "ubpf.h"

#ifdef __cplusplus
}
#endif

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Ubpf {

Filter::Filter() {
  ENVOY_LOG_MISC(debug, "ubpf: init");
  auto* vm = ubpf_create();
  ENVOY_LOG_MISC(debug, "ubpf: VM created");

  ubpf_destroy(vm);
  ENVOY_LOG_MISC(debug, "ubpf: VM destroyed");
}

} // namespace Ubpf
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
