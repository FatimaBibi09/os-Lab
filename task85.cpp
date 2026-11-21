// Task 10: Cleaning zombies asynchronously with a SIGCHLD handler
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

volatile sig_atomic_t reaped_count = 0;
const int NUM_CHILDREN = 5;

void sigchld_handler(int) {
    int status;
    const char msg[] = "Asynchronously reaped a terminated child.\n";
    // Drain all terminated children (signals can be coalesced)
    while (waitpid(-1, &status, WNOHANG) > 0) {
        write(STDOUT_FILENO, msg, sizeof(msg) - 1);
        reaped_count++;
    }
}

void do_work() { printf("Parent doing primary work... (reaped so far: %d)\n", (int)reaped_count); }

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    // Block SIGCHLD so it cannot slip in between the check and sigsuspend()
    sigset_t block, old;
    sigemptyset(&block);
    sigaddset(&block, SIGCHLD);
    sigprocmask(SIG_BLOCK, &block, &old);

    srand(time(NULL));
    for (int i = 1; i <= NUM_CHILDREN; i++) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); exit(1); }
        if (pid == 0) {
            sigprocmask(SIG_SETMASK, &old, NULL);
            sleep(rand() % 3 + 1);
            exit(0);
        }
    }

    // Parent continues primary processing without blocking on any one child
    while (reaped_count < NUM_CHILDREN) {
        do_work();
        fflush(stdout);
        sigsuspend(&old);               // like pause(), but race-free
    }
    printf("All %d children reaped. No zombies left.\n", NUM_CHILDREN);
    return 0;
}
