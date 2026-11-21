// Task 2: Memory isolation (copy-on-write)
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>

int counter = 100;

int main() {
    pid_t pid = fork();
    if (pid < 0) { perror("Fork failed"); return 1; }

    if (pid == 0) {
        counter += 50;                  // modifies the child's own copy
        printf("[Child]  counter = %d at address: %p\n", counter, (void*)&counter);
        exit(0);
    } else {
        sleep(1);                       // give child time to modify its variable
        printf("[Parent] counter = %d at address: %p\n", counter, (void*)&counter);
        wait(NULL);
    }
    // Addresses look identical (virtual), but the values stay isolated.
    return 0;
}
