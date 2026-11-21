// Task 1: Basic POSIX Thread Lifecycle (create, join, exit)
#include <pthread.h>
#include <unistd.h>
#include <cstdio>
#include <cassert>

void* worker_routine(void* arg) {
    int thread_arg = *(int*)arg;
    printf("Worker executing with Arg: %d, POSIX TID: %lu\n",
           thread_arg, (unsigned long)pthread_self());
    usleep(100000);                 // perform_light_computation()
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[4];
    int args[4] = {1, 2, 3, 4};

    for (int i = 0; i < 4; i++) {
        int status = pthread_create(&threads[i], NULL, worker_routine, &args[i]);
        assert(status == 0);
    }
    for (int i = 0; i < 4; i++)
        pthread_join(threads[i], NULL);

    printf("All kernel worker threads completed successfully.\n");
    return 0;
}
