#include "source/extensions/filters/http/ubpf/ubpf_filter.h"

#include "source/common/common/logger.h"

#include "ubpf.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Ubpf {

Filter::Filter(Api::Api& api) {
  ENVOY_LOG_MISC(debug, "ubpf: init");
  auto* vm = ubpf_create();
  ENVOY_LOG_MISC(debug, "ubpf: VM created");


  const std::string elf_contents = api.fileSystem().fileReadToEnd("hello.o");
  ENVOY_LOG_MISC(debug, "ubpf: ELF size {} bytes", elf_contents.size());

  {
    char *err = nullptr;
    const int ret = ubpf_load_elf(vm, elf_contents.data(), elf_contents.size(), &err);
    RELEASE_ASSERT(ret == 0, "uBPF ELF load failure");
  }

  {
    uint64_t bpf_ret_value = 0;
    const int ret = ubpf_exec(vm, nullptr, 0, &bpf_ret_value);
    RELEASE_ASSERT(ret == 0, "uBPF exec failure");
    ENVOY_LOG_MISC(debug, "ubpf: output {}", bpf_ret_value);
  }

  ubpf_destroy(vm);
  ENVOY_LOG_MISC(debug, "ubpf: VM destroyed");
}

} // namespace Ubpf
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
