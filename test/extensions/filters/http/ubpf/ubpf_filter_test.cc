#include <cstdint>
#include <memory>

#include "source/common/buffer/buffer_impl.h"
#include "source/common/http/message_impl.h"
#include "source/common/stream_info/stream_info_impl.h"
#include "source/extensions/filters/http/ubpf/lua_filter.h"

#include "test/mocks/api/mocks.h"
#include "test/mocks/http/mocks.h"
#include "test/mocks/network/mocks.h"
#include "test/mocks/server/factory_context.h"
#include "test/mocks/ssl/mocks.h"
#include "test/mocks/thread_local/mocks.h"
#include "test/mocks/upstream/cluster_manager.h"
#include "test/test_common/logging.h"
#include "test/test_common/printers.h"
#include "test/test_common/utility.h"

#include "gmock/gmock.h"

using testing::_;
using testing::AtLeast;
using testing::Eq;
using testing::HasSubstr;
using testing::InSequence;
using testing::Invoke;
using testing::Return;
using testing::ReturnRef;
using testing::StrEq;

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Ubpf {
namespace {

// Script asking for body chunks, request that is headers only.
TEST_F(UbpfHttpFilterTest, DecodeHeaders) {
  InSequence s;
  setup(BODY_CHUNK_SCRIPT);

  Http::TestRequestHeaderMapImpl request_headers{{":path", "/"}};
  EXPECT_CALL(*filter_, scriptLog(spdlog::level::trace, StrEq("/")));
  EXPECT_CALL(*filter_, scriptLog(spdlog::level::trace, StrEq("done")));
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->decodeHeaders(request_headers, true));
}


class UbpfHttpFilterTest : public testing::Test {
public:
  UbpfHttpFilterTest() {}

  ~UbpfHttpFilterTest() override { filter_->onDestroy(); }

  // Quickly set up a global configuration. In order to avoid extensive modification of existing
  // test cases, the existing configuration methods must be compatible.
  void setup(const std::string& path) {
    envoy::extensions::filters::http::ubpf::v3::Ubpf proto_config;
    proto_config.set_filename(path);

    filter_ = std::make_unique<TestFilter>(config_, test_time.timeSystem());
    filter_->setDecoderFilterCallbacks(decoder_callbacks_);
    filter_->setEncoderFilterCallbacks(encoder_callbacks_);
  }

  NiceMock<Server::Configuration::MockServerFactoryContext> server_factory_context_;
  NiceMock<Api::MockApi> api_;
  std::unique_ptr<Filter> filter_;
  Http::MockStreamDecoderFilterCallbacks decoder_callbacks_;
};

} // namespace
} // namespace Ubpf
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
