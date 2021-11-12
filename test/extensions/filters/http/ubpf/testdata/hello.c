static int idouble(int a) {
        return (a * 2);
}

int bpf_prog(void *ctx) {
        int a = 2;
        a = idouble(a);

        return (a);
}
