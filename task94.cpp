// Task 19: Timeout watchdog - kill a runaway child with SIGKILL
// Usage: ./task19 [timeout_seconds]   (default 3)
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>

int main(int argc, char** argv) {
    int timeout = (argc > 1) ? atoi(argv[1]) : 3;
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) {
        volatile unsigned long x = 0;
        while (true) { x++; }           // run long task (never ends)
    } else {
        sleep(timeout);                 // allowed execution threshold
        int status;
        pid_t check = waitpid(pid, &status, WNOHANG);
        if (check == 0) {
            printf("Child exceeded time limit. Dispatching SIGKILL to PID: %d\n", pid);
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);   // reap terminated process
            if (WIFSIGNALED(status))
                printf("Child reaped: killed by signal %d\n", WTERMSIG(status));
        } else {
            printf("Child finished within the time limit.\n");
        }
    }
    return 0;
}
