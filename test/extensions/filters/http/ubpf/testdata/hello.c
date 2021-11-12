static int idouble(int a) {
        return (a * 2);
}

int bpf_prog(void *ctx) {
        int a = 1;
        a = idouble(a);

        return (a);
}
