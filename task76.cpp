// Task 1: Basic fork and identity verification
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        printf("Child Process -> PID: %d | Parent PID: %d\n", getpid(), getppid());
        exit(0);
    } else {
        printf("Parent Process -> PID: %d | Child PID: %d\n", getpid(), pid);
        wait(NULL);                     // wait for child completion
    }
    return 0;
}
