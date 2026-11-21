// Task 11: Preventing zombies with SIG_IGN + SA_NOCLDWAIT
// Usage: ./task11 [parent_sleep_seconds]   (default 10)
// Check with: ps aux | grep Z   -> the child never shows up as Z.
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

int main(int argc, char** argv) {
    int secs = (argc > 1) ? atoi(argv[1]) : 10;

    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDWAIT;
    sigaction(SIGCHLD, &sa, NULL);

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) {
        printf("Child terminating immediately.\n");
        exit(0);
    }

    // Parent sleeps; the kernel deallocates the child's PCB automatically
    printf("Parent sleeping %ds. Child PID %d should NOT appear as defunct.\n", secs, pid);
    fflush(stdout);
    sleep(secs);

    // Proof: there is nothing left to wait for
    if (waitpid(pid, NULL, 0) == -1 && errno == ECHILD)
        printf("waitpid -> ECHILD: child was auto-reaped by the kernel.\n");
    return 0;
}
