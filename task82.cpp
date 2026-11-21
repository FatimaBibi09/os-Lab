// Task 7: Targeted waiting with waitpid()
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>

int main() {
    pid_t pidA = fork();
    if (pidA < 0) { perror("fork"); return 1; }
    if (pidA == 0) { sleep(4); exit(10); }

    pid_t pidB = fork();
    if (pidB < 0) { perror("fork"); return 1; }
    if (pidB == 0) { sleep(1); exit(20); }

    int status;
    // Wait for Child A first, even though Child B finishes earlier
    // (B becomes a zombie for ~3 seconds meanwhile).
    waitpid(pidA, &status, 0);
    printf("Child A reaped with status: %d\n", WEXITSTATUS(status));

    waitpid(pidB, &status, 0);
    printf("Child B reaped with status: %d\n", WEXITSTATUS(status));
    return 0;
}
