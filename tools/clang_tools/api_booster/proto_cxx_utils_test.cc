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
  EXPECT_EQ("", ProtoCxxUtils::protoToCxxType("", false, false));
  EXPECT_EQ("", ProtoCxxUtils::protoToCxxType("", true, false));
  EXPECT_EQ("foo", ProtoCxxUtils::protoToCxxType("foo", false, false));
  EXPECT_EQ("foo", ProtoCxxUtils::protoToCxxType("foo", true, false));
  EXPECT_EQ("bar", ProtoCxxUtils::protoToCxxType("foo.bar", false, false));
  EXPECT_EQ("foo::bar", ProtoCxxUtils::protoToCxxType("foo.bar", true, false));
  EXPECT_EQ("foo::Bar", ProtoCxxUtils::protoToCxxType("foo.Bar", true, false));
  EXPECT_EQ("foo", ProtoCxxUtils::protoToCxxType("foo.Bar", true, true));
  EXPECT_EQ("foo::Bar_Baz", ProtoCxxUtils::protoToCxxType("foo.Bar.Baz", true, false));
  EXPECT_EQ("foo::Bar_Baz_Blah", ProtoCxxUtils::protoToCxxType("foo.Bar.Baz.Blah", true, false));
  EXPECT_EQ("foo::Bar_Baz", ProtoCxxUtils::protoToCxxType("foo.Bar.Baz.Blah", true, true));
}

} // namespace
} // namespace ApiBooster
