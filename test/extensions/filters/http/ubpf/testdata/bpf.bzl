# Temporary fork of https://github.com/jackhumphries/bazel-ebpf/blob/main/bpf.bzl
def bpf_program(name, src, hdrs, bpf_object, **kwargs):
    """Generates an eBPF object file from .c source code.
    Args:
      name: target name for eBPF program.
      src: eBPF program source code in C.
      hdrs: list of header files depended on by src.
      bpf_object: name of generated eBPF object file.
      **kwargs: additional arguments.
    """
    native.genrule(
        name = name,
        srcs = [src] + hdrs,
        outs = [bpf_object],
        cmd = (
            "clang -g -O2 -target bpf -D__TARGET_ARCH_x86 " +
            # The `.` directory is the project root, so we pass it with the `-I`
            # flag so that #includes work in the source files.
            #
            # `$@` is the location to write the eBPF object file.
            "-I . -c $(location " + src + ") -o $@ && " +
            "llvm-strip -g $@"
        ),
        **kwargs
    )
