// Task 14: The 1:N blocking I/O pitfall
//   ./task14         -> ULT version: heartbeat FREEZES while read() blocks
//   ./task14 klt     -> pthread version: heartbeat keeps ticking
// Press Enter when asked to finish the read.
#include "ult.h"
#include <pthread.h>
#include <unistd.h>
#include <atomic>
#include <string>

std::atomic<bool> io_done(false);

// ---------------- ULT version ----------------
void ult_thread_heartbeat(void*) {
    for (int i = 1; !io_done && i <= 8; i++) {
        printf("ULT 2: Heartbeat tick %d (active)\n", i);
        fflush(stdout);
        sleep(1);
        my_thread_yield();
    }
}

void ult_thread_blocking(void*) {
    printf("ULT 1: Waiting on blocking I/O (read standard input)... press Enter\n");
    fflush(stdout);
    char buffer[100];
    read(STDIN_FILENO, buffer, 100);    // blocks the WHOLE process (kernel sees 1 thread)
    printf("ULT 1: Received I/O!\n");
    io_done = true;
}

// ---------------- pthread version ----------------
void* pthread_heartbeat(void*) {
    for (int i = 1; !io_done && i <= 8; i++) {
        printf("KLT 2: Heartbeat tick %d (active)\n", i);
        fflush(stdout);
        sleep(1);
    }
    return NULL;
}

void* pthread_blocking(void*) {
    printf("KLT 1: Waiting on blocking I/O... press Enter\n");
    fflush(stdout);
    char buffer[100];
    read(STDIN_FILENO, buffer, 100);    // blocks only this thread
    printf("KLT 1: Received I/O!\n");
    io_done = true;
    return NULL;
}

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "klt") {
        pthread_t a, b;
        pthread_create(&a, NULL, pthread_heartbeat, NULL);
        pthread_create(&b, NULL, pthread_blocking, NULL);
        pthread_join(a, NULL);
        pthread_join(b, NULL);
    } else {
        ult_create(ult_thread_heartbeat, NULL);
        ult_create(ult_thread_blocking, NULL);
        scheduler_loop();
    }
    return 0;
}
