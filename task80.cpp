// Task 5: Basic synchronization with wait()
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>

int main() {
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) {
        for (int i = 1; i <= 5; i++) {
            printf("Child working... step %d\n", i);
            fflush(stdout);
            usleep(500000);
        }
        exit(0);
    } else {
        printf("Parent waiting for child completion...\n");
        fflush(stdout);
        wait(NULL);                     // blocks until the child finishes
        printf("Parent resumed: Child terminated successfully.\n");
    }
    return 0;
}
