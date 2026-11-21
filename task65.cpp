// Task 10: Round-robin cooperative ULT scheduler (see ult.h for the TCB,
// ready_queue, my_thread_yield() and scheduler_loop() implementation)
#include "ult.h"

void worker(void* arg) {
    int name = (int)(long)arg;
    for (int step = 1; step <= 4; step++) {
        printf("ULT %d running step %d\n", name, step);
        my_thread_yield();              // cooperative: give the CPU to the next ULT
    }
    printf("ULT %d finished\n", name);
}

int main() {
    for (long i = 1; i <= 3; i++) ult_create(worker, (void*)i);
    scheduler_loop();
    printf("All user-level threads terminated.\n");
    return 0;
}
