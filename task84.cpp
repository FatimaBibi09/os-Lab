// Task 9: Deliberate zombie process
// Usage: ./task09 [parent_sleep_seconds]   (default 20)
// In another terminal run:  ps aux | grep Z   (look for state Z / <defunct>)
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>

int main(int argc, char** argv) {
    int secs = (argc > 1) ? atoi(argv[1]) : 20;
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) {
        printf("Child exiting immediately (PID: %d)\n", getpid());
        exit(0);                        // becomes a zombie: parent has not reaped it yet
    } else {
        printf("Parent sleeping %ds. Run: ps aux | grep Z   (child PID %d)\n", secs, pid);
        fflush(stdout);
        sleep(secs);
        wait(NULL);                     // zombie is finally reaped and deallocated here
        printf("Zombie process reaped.\n");
    }
    return 0;
}
