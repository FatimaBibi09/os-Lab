// Task 14: Process image replacement with execlp
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>

int main() {
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) {
        printf("Child executing 'ls -la'...\n");
        fflush(stdout);                 // flush before the image is replaced
        execlp("ls", "ls", "-la", (char*)NULL);
        // Unreachable if exec succeeds:
        perror("execlp failed");
        exit(1);
    } else {
        wait(NULL);
        printf("Parent: Child execution completed.\n");
    }
    return 0;
}
