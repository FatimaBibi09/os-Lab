// Task 12: User-level mutex (no OS calls). See UserMutex in ult.h.
#include "ult.h"

UserMutex lock;
int shared_counter = 0;

void worker(void* arg) {
    int id = (int)(long)arg;
    for (int i = 0; i < 3; i++) {
        user_mutex_lock(&lock);
        int tmp = shared_counter;
        printf("ULT %d got the lock, counter = %d\n", id, tmp);
        my_thread_yield();              // yield INSIDE the critical section: others must block
        shared_counter = tmp + 1;
        user_mutex_unlock(&lock);
        my_thread_yield();
    }
}

int main() {
    for (long i = 1; i <= 3; i++) ult_create(worker, (void*)i);
    scheduler_loop();
    printf("Final counter = %d (expected 9)\n", shared_counter);
    return 0;
}
