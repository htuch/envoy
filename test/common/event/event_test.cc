#include <fcntl.h>
#include <unistd.h>

#include "event2/event.h"
#include "event2/event_struct.h"
#include "gtest/gtest.h"

namespace Envoy {
namespace {

void eventCb(evutil_socket_t, short what, void* arg) {
  bool* activated = static_cast<bool*>(arg);
  *activated = true;
  std::cerr << "event is " << what << std::endl;
  EXPECT_EQ(what, EV_ET); // we're only seeing EV_ET, not EV_CLOSED.
  ASSERT_TRUE(false);
}

TEST(EventTest, PhantomEvent) {
  event_base* base = event_base_new();
  event raw_event;
  bool activated = false;
  int fds[2];
  ASSERT_EQ(0, ::pipe2(fds, O_NONBLOCK));
  event_assign(&raw_event, base, fds[0], EV_ET | EV_CLOSED, eventCb,
               &activated);
  event_add(&raw_event, nullptr);
  ASSERT_EQ(0, ::close(fds[1]));
  event_base_loop(base, EVLOOP_NONBLOCK);
  EXPECT_FALSE(activated);
  event_base_free(base);
} // namespace

} // namespace
} // namespace Envoy
