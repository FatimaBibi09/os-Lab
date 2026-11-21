// Task 3: Thread creation with the raw clone() system call (no pthread)
#include <sched.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>

const int STACK_SIZE = 65536;       // 64 KB

static int child_execution(void*) {
    char buf[100];
    // write() instead of printf: this thread has no proper libc TLS setup
    int n = snprintf(buf, sizeof buf, "Inside child clone thread. Kernel TID: %ld\n",
                     (long)syscall(SYS_gettid));
    write(1, buf, n);
    return 0;
}

int main() {
    char* child_stack = (char*)malloc(STACK_SIZE);
    char* stack_top = child_stack + STACK_SIZE;     // stacks grow downwards on x86

    int clone_flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;

    int tid = clone(child_execution, stack_top, clone_flags, NULL);
    if (tid == -1) { perror("Clone creation failed"); return 1; }

    printf("Spawned kernel thread with TID: %d\n", tid);
    fflush(stdout);
    sleep(1);                       // CLONE_THREAD children can't be waited on, so just sleep
    free(child_stack);
    return 0;
}
