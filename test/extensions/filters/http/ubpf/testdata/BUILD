load(
    "//bazel:envoy_build_system.bzl",
    "envoy_package",
)
load(":bpf.bzl", "bpf_program")

licenses(["notice"])  # Apache 2

envoy_package()


bpf_program(
    name = "hello",
    src = "hello.c",
    hdrs = [],
    bpf_object = "hello.o",
)
