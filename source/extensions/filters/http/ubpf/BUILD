load(
    "//bazel:envoy_build_system.bzl",
    "envoy_cc_extension",
    "envoy_cc_library",
    "envoy_extension_package",
)

licenses(["notice"])  # Apache 2

envoy_extension_package()

envoy_cc_library(
    name = "ubpf_filter_lib",
    #srcs = ["ubpf_filter.cc"],
    hdrs = ["ubpf_filter.h"],
    deps = [
        "//external:ubpf",
        "//envoy/http:filter_interface",
    ],
)

envoy_cc_extension(
    name = "config",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    deps = [
        ":ubpf_filter_lib",
        "//envoy/registry",
        "//source/extensions/filters/http/common:factory_base_lib",
        "@envoy_api//envoy/extensions/filters/http/ubpf/v3:pkg_cc_proto",
    ],
)
