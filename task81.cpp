// Task 6: Inspecting child exit status with POSIX macros
// Usage: ./task06            -> child exits with code 42
//        ./task06 crash      -> child kills itself with SIGSEGV (shows WIFSIGNALED)
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>

int main(int argc, char** argv) {
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) {
        if (argc > 1 && strcmp(argv[1], "crash") == 0) raise(SIGSEGV);
        exit(42);                       // explicit non-zero exit status
    } else {
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            printf("Child exited normally with status: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child killed by signal: %d (%s)\n", WTERMSIG(status), strsignal(WTERMSIG(status)));
        }
    }
    return 0;
}
