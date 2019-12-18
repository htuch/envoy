#include "gtest/gtest.h"
#include "proto_cxx_utils.h"

namespace ApiBooster {
namespace {

// Validate C++ to proto type name conversion.
TEST(ProtoCxxUtils, CxxToProtoType) {
  EXPECT_EQ("", ProtoCxxUtils::cxxToProtoType(""));
  EXPECT_EQ("foo", ProtoCxxUtils::cxxToProtoType("foo"));
  EXPECT_EQ("foo.bar", ProtoCxxUtils::cxxToProtoType("foo::bar"));
  EXPECT_EQ("foo.bar", ProtoCxxUtils::cxxToProtoType("foo::bar::FooCase"));
  EXPECT_EQ("foo.bar.Baz.Blah", ProtoCxxUtils::cxxToProtoType("foo::bar::Baz_Blah"));
}

// Validate proto to C++ type name conversion.
TEST(ProtoCxxUtils, ProtoToCxxType) {
  EXPECT_EQ("", ProtoCxxUtils::protoToCxxType("", false));
  EXPECT_EQ("", ProtoCxxUtils::protoToCxxType("", true));
  EXPECT_EQ("foo", ProtoCxxUtils::protoToCxxType("foo", false));
  EXPECT_EQ("foo", ProtoCxxUtils::protoToCxxType("foo", true));
  EXPECT_EQ("bar", ProtoCxxUtils::protoToCxxType("foo.bar", false));
  EXPECT_EQ("foo::bar", ProtoCxxUtils::protoToCxxType("foo.bar", true));
}

} // namespace
} // namespace ApiBooster
