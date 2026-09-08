/* Verification-only kernel-query failures and declared capacity boundaries. */
static unsigned test_probe_mode, test_probe_infos;
static int test_proc_listpids(uint32_t type, uint32_t group, void *buffer, int capacity) {
    if (!test_probe_mode) return proc_listpids(type, group, buffer, capacity);
    if (test_probe_mode == 3) return 1;
    if (test_probe_mode == 4) { errno = EPERM; return 0; }
    unsigned count = test_probe_mode == 1 ? NATIVE_PROCESS_LIMIT : test_probe_mode == 2 ? NATIVE_PROCESS_LIMIT+1 : 1;
    assert((size_t)capacity >= count * sizeof(pid_t));
    for (unsigned i = 0; i < count; ++i) ((pid_t *)buffer)[i] = (pid_t)(1000000+i);
    return (int)(count * sizeof(pid_t));
}
static int test_proc_pidinfo(int pid, int flavor, uint64_t argument, void *buffer, int size) {
    if (!test_probe_mode) return proc_pidinfo(pid, flavor, argument, buffer, size);
    ++test_probe_infos;
    if (test_probe_mode == 1) { errno = ESRCH; return 0; }
    if (test_probe_mode == 5) { errno = EPERM; return 0; }
    assert(size == sizeof(struct proc_bsdinfo));
    struct proc_bsdinfo *info = buffer;
    memset(info, 0, sizeof(*info));
    info->pbi_pgid = test_probe_mode == 8 ? 12346 : 12345;
    info->pbi_status = test_probe_mode == 6 ? SZOMB : SRUN;
    return sizeof(*info);
}
#define proc_listpids test_proc_listpids
#define proc_pidinfo test_proc_pidinfo
