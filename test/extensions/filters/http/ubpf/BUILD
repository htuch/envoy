load(
    "//bazel:envoy_build_system.bzl",
    "envoy_package",
)
load(
    "//test/extensions:extensions_build_system.bzl",
    "envoy_extension_cc_test",
)

licenses(["notice"])  # Apache 2

envoy_package()

envoy_extension_cc_test(
    name = "ubpf_filter_test",
    srcs = ["ubpf_filter_test.cc"],
    data = [
        "//test/extensions/filters/http/ubpf/testdata:hello",
    ],
    extension_names = ["envoy.filters.http.ubpf"],
    deps = [
        "//source/extensions/filters/http/ubpf:ubpf_filter_lib",
        "//test/test_common:utility_lib",
    ],
)
