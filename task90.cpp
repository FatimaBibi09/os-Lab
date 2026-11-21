// Task 15: execve with an argument vector and a custom environment
// Build reporter first:  g++ reporter.cpp -o reporter   (make does this)
// Run from the same folder: ./task15_execve
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>

int main() {
    char* args[] = {(char*)"reporter", (char*)"arg1", (char*)"arg2", NULL};
    char* envs[] = {(char*)"LAB_MODULE=OS_PROC", (char*)"SEM=FALL2026", NULL};

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) {
        execve("./reporter", args, envs);
        perror("execve execution failed");
        exit(1);
    }
    wait(NULL);
    printf("Parent: reporter finished.\n");
    return 0;
}
