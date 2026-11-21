// Task 8: Non-blocking wait (WNOHANG)
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>

int main() {
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) { sleep(3); exit(0); }

    int status;
    while (true) {
        pid_t result = waitpid(pid, &status, WNOHANG);
        if (result == 0) {
            printf("Parent computing independent background tasks...\n");
            usleep(500000);
        } else if (result == pid) {
            printf("Child finished! Parent wrapping up.\n");
            break;
        } else {
            perror("waitpid");
            break;
        }
    }
    return 0;
}
