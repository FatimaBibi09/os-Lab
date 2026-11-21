// Task 12: Orphan process (adopted by init/systemd)
#include <unistd.h>
#include <cstdio>
#include <cstdlib>

int main() {
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) {
        printf("Child started. Initial Parent PID: %d\n", getppid());
        fflush(stdout);
        sleep(2);                       // wait for the original parent to exit
        printf("Child after parent exit. New Parent PID: %d\n", getppid());
        exit(0);
    } else {
        printf("Parent exiting immediately. Child is now orphaned.\n");
        exit(0);
    }
}
