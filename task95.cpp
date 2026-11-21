// Task 20: Process lifecycle monitor - children ending in different ways
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>

int main() {
    const int N = 3;
    pid_t kids[N];

    for (int i = 0; i < N; i++) {
        kids[i] = fork();
        if (kids[i] < 0) { perror("fork"); return 1; }
        if (kids[i] == 0) {
            usleep(200000 * (i + 1));   // stagger so output is easy to read
            if (i == 0) exit(0);                    // Child 1: clean exit(0)
            if (i == 1) exit(15);                   // Child 2: custom exit code
            raise(SIGSEGV);                         // Child 3: crash with SIGSEGV
        }
        printf("Parent started child %d with PID %d\n", i + 1, kids[i]);
        fflush(stdout);                 // flush so forked children do not copy buffered text
    }
    fflush(stdout);

    // Non-blocking harvest loop: parent stays free to do other work between polls
    int status, alive = N;
    while (alive > 0) {
        pid_t reaped_pid = waitpid(-1, &status, WNOHANG);
        if (reaped_pid > 0) {
            alive--;
            printf("Harvested PID: %d\n", reaped_pid);
            if (WIFEXITED(status)) {
                printf("  -> Clean termination with exit code: %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("  -> Crashed / killed by signal: %d (%s)\n",
                       WTERMSIG(status), strsignal(WTERMSIG(status)));
            }
        } else if (reaped_pid == 0) {
            usleep(100000);             // no one finished yet: do other work
        } else {
            perror("waitpid");
            break;
        }
    }
    printf("All children harvested.\n");
    return 0;
}
