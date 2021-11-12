#include "source/extensions/filters/http/ubpf/lua_filter.h"

#include "test/mocks/api/mocks.h"

#include "gmock/gmock.h"

using testing::InSequence;

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Ubpf {
namespace {

class UbpfHttpFilterTest : public testing::Test {
public:
  UbpfHttpFilterTest() {}

  ~UbpfHttpFilterTest() override { filter_->onDestroy(); }

  // Quickly set up a global configuration. In order to avoid extensive modification of existing
  // test cases, the existing configuration methods must be compatible.
  void setup(const std::string& path) {
    filter_ = std::make_unique<Filter>(api_, path);
  }

  NiceMock<Api::MockApi> api_;
  std::unique_ptr<Filter> filter_;
};

TEST_F(UbpfHttpFilterTest, DecodeHeaders) {
  InSequence s;
  setup("hello.o");

  Http::TestRequestHeaderMapImpl request_headers{{":path", "/"}};
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->decodeHeaders(request_headers, true));
}

} // namespace
} // namespace Ubpf
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
