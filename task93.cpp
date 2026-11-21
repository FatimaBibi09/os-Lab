// Task 18: File descriptor inheritance across fork (shared file offset)
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

int main() {
    int fd = open("log.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return 1; }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) {
        const char* msg = "[Child Data]\n";
        write(fd, msg, strlen(msg));
        close(fd);
        exit(0);
    } else {
        wait(NULL);
        const char* msg = "[Parent Data]\n";
        write(fd, msg, strlen(msg));    // continues after the child's text: shared offset
        close(fd);
    }
    // Both lines appear in log.txt without overwriting each other
    printf("Done. Contents of log.txt:\n");
    fflush(stdout);
    system("cat log.txt");
    return 0;
}
