#include "source/extensions/filters/http/ubpf/ubpf_filter.h"

#include "source/common/common/logger.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Ubpf {

Filter::Filter(Api::Api& api, const std::string& path) {
  ENVOY_LOG_MISC(debug, "ubpf: init");
  vm_ = ubpf_create();
  ENVOY_LOG_MISC(debug, "ubpf: loading BPF code from '{}'", path);

  const std::string elf_contents = api.fileSystem().fileReadToEnd(path);
  ENVOY_LOG_MISC(debug, "ubpf: ELF size {} bytes", elf_contents.size());

  char* err = nullptr;
  const int ret = ubpf_load_elf(vm_, elf_contents.data(), elf_contents.size(), &err);
  RELEASE_ASSERT(ret == 0, "uBPF ELF load failure");
}

Http::FilterHeadersStatus Filter::decodeHeaders(Http::RequestHeaderMap& /*headers*/,
                                                bool /*end_stream*/) {
  uint64_t bpf_ret_value = 0;
  const int ret = ubpf_exec(vm_, nullptr, 0, &bpf_ret_value);
  RELEASE_ASSERT(ret == 0, "uBPF exec failure");
  ENVOY_LOG_MISC(debug, "ubpf: output {}", bpf_ret_value);

  return Http::FilterHeadersStatus::Continue;
}

void Filter::onDestroy() {
  ubpf_destroy(vm_);
  ENVOY_LOG_MISC(debug, "ubpf: VM destroyed");
}

} // namespace Ubpf
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
