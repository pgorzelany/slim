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

/* Child waits until the parent has made (or deliberately declined) the exact
   group. This prevents scheduling from invalidating the wrong-group control. */
static unsigned test_group_mode;
static int test_group_gate[2] = {-1, -1};
static int test_setpgid(pid_t pid, pid_t group) {
    if (!test_group_mode) return setpgid(pid, group);
    if (pid == 0) {
        assert(group == 0 && test_group_gate[0] >= 0 && test_group_gate[1] >= 0);
        assert(close(test_group_gate[1]) == 0);
        char ready;
        ssize_t count;
        do { count = read(test_group_gate[0], &ready, 1); } while (count < 0 && errno == EINTR);
        assert(count == 1 && ready == 'g' && close(test_group_gate[0]) == 0);
        assert((getpgrp() == getpid()) == (test_group_mode != 2));
        errno = test_group_mode == 3 ? EIO : EPERM;
        return -1;
    }
    assert(pid == group && test_group_gate[0] >= 0 && test_group_gate[1] >= 0);
    int result;
    if (test_group_mode == 2) {
        result = -1; errno = EPERM;
    } else {
        result = setpgid(pid, group);
        assert(result == 0);
    }
    int saved = errno;
    assert(close(test_group_gate[0]) == 0);
    ssize_t count;
    do { count = write(test_group_gate[1], "g", 1); } while (count < 0 && errno == EINTR);
    assert(count == 1 && close(test_group_gate[1]) == 0);
    test_group_gate[0] = test_group_gate[1] = -1;
    errno = saved;
    return result;
}
#define setpgid test_setpgid
static pid_t test_waitpid(pid_t pid, int *status, int options) {
    pid_t result = waitpid(pid, status, options);
    if (result == pid && test_group_mode) {
        assert(WIFEXITED(*status));
        assert(WEXITSTATUS(*status) == (test_group_mode == 1 ? 0 : 126));
    }
    return result;
}
#define waitpid test_waitpid
