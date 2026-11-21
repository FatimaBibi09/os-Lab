// Task 3: Two sibling children under one parent
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>

int main() {
    pid_t c1 = fork();
    if (c1 < 0) { perror("fork"); return 1; }
    if (c1 == 0) {
        printf("First Sibling running with PID: %d\n", getpid());
        exit(0);                        // exit so Child 1 never reaches the 2nd fork
    }

    // Only the parent runs here
    pid_t c2 = fork();
    if (c2 < 0) { perror("fork"); return 1; }
    if (c2 == 0) {
        printf("Second Sibling running with PID: %d\n", getpid());
        exit(0);
    }

    wait(NULL);                         // parent reaps both siblings
    wait(NULL);
    printf("Parent %d reaped both children.\n", getpid());
    return 0;
}
