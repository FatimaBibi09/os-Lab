// Task 4: Linear process chain P1 -> P2 -> P3 (bottom-up termination)
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>

int main() {
    pid_t p2 = fork();
    if (p2 < 0) { perror("fork"); return 1; }
    if (p2 == 0) {
        // Child 1 context
        pid_t p3 = fork();
        if (p3 < 0) { perror("fork"); exit(1); }
        if (p3 == 0) {
            printf("[Grandchild] PID: %d | Parent: %d\n", getpid(), getppid());
            exit(0);
        }
        wait(NULL);                     // Child 1 waits for Grandchild
        printf("[Child 1]    PID: %d | Parent: %d\n", getpid(), getppid());
        exit(0);
    }
    wait(NULL);                         // Parent waits for Child 1
    printf("[Parent]     All descendants have completed.\n");
    return 0;
}
