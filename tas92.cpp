// Task 17: Recursive binary process tree
// Usage: ./task17 [max_depth]   (default 2 -> 6 child processes; depth 3 -> 14)
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>

void build_tree(int level, int max_depth) {
    if (level >= max_depth) return;

    pid_t left = fork();
    if (left < 0) { perror("fork"); exit(1); }
    if (left == 0) {
        printf("Level %d Left  Child: %d (parent %d)\n", level + 1, getpid(), getppid());
        fflush(stdout);
        build_tree(level + 1, max_depth);
        exit(0);
    }

    pid_t right = fork();
    if (right < 0) { perror("fork"); exit(1); }
    if (right == 0) {
        printf("Level %d Right Child: %d (parent %d)\n", level + 1, getpid(), getppid());
        fflush(stdout);
        build_tree(level + 1, max_depth);
        exit(0);
    }

    waitpid(left, NULL, 0);
    waitpid(right, NULL, 0);
}

int main(int argc, char** argv) {
    int depth = (argc > 1) ? atoi(argv[1]) : 2;
    printf("Root: %d, tree depth %d\n", getpid(), depth);
    fflush(stdout);
    build_tree(0, depth);
    printf("Root: all nodes finished.\n");
    return 0;
}
