// Task 13: Subreaper adoption with prctl(PR_SET_CHILD_SUBREAPER)
#include <unistd.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <cstdio>
#include <cstdlib>

int main() {
    // Process A registers as subreaper: orphaned descendants are re-parented to A, not PID 1
    if (prctl(PR_SET_CHILD_SUBREAPER, 1) == -1) { perror("prctl"); return 1; }
    printf("Process A PID: %d (subreaper)\n", getpid());
    fflush(stdout);

    pid_t b = fork();
    if (b < 0) { perror("fork"); return 1; }
    if (b == 0) {
        // Process B
        pid_t c = fork();
        if (c < 0) { perror("fork"); exit(1); }
        if (c == 0) {
            // Process C
            sleep(1);                   // wait for Process B to exit
            printf("Process C adopted by Subreaper PPID: %d\n", getppid());
            exit(0);
        }
        exit(0);                        // Process B exits immediately, orphaning C
    }

    // Process A reaps both B and the adopted descendant C
    wait(NULL);
    wait(NULL);
    printf("Process A reaped B and adopted C.\n");
    return 0;
}
