// Task 2: Kernel Task ID (gettid) vs POSIX thread handle (pthread_self)
// Usage: ./task02 [sleep_seconds]   (default 20, so you can run `ps -eLf` or `htop`)
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <cstdio>
#include <cstdlib>

int sleep_secs = 20;

void* thread_inspect(void*) {
    unsigned long posix_id = (unsigned long)pthread_self();
    long kernel_tid = syscall(SYS_gettid);
    int process_pid = getpid();
    printf("PID: %d | POSIX ID: %lu | Kernel TID: %ld\n", process_pid, posix_id, kernel_tid);
    fflush(stdout);
    sleep(sleep_secs);              // time to inspect with: ps -eLf | grep task02
    return NULL;
}

int main(int argc, char** argv) {
    if (argc > 1) sleep_secs = atoi(argv[1]);
    pthread_t threads[3];
    for (int i = 0; i < 3; i++) pthread_create(&threads[i], NULL, thread_inspect, NULL);
    for (int i = 0; i < 3; i++) pthread_join(threads[i], NULL);
    return 0;
}
