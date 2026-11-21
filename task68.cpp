// Task 13: Producer-consumer on user-level threads (single kernel thread)
#include "ult.h"

const int ITEMS = 10;                   // stop after 10 items so the demo ends
int user_items = 0;                     // current items in the 5-slot buffer
UserMutex queue_lock;

void user_producer(void*) {
    int produced = 0;
    while (produced < ITEMS) {
        user_mutex_lock(&queue_lock);
        if (user_items < 5) {
            user_items++;
            produced++;
            printf("Produced user item. Total: %d\n", user_items);
        }
        user_mutex_unlock(&queue_lock);
        my_thread_yield();              // cooperative back-off
    }
}

void user_consumer(void*) {
    int consumed = 0;
    while (consumed < ITEMS) {
        user_mutex_lock(&queue_lock);
        if (user_items > 0) {
            user_items--;
            consumed++;
            printf("    Consumed user item. Total: %d\n", user_items);
        }
        user_mutex_unlock(&queue_lock);
        my_thread_yield();
    }
}

int main() {
    ult_create(user_producer, NULL);
    ult_create(user_consumer, NULL);
    scheduler_loop();
    printf("Pipeline finished. Items left: %d\n", user_items);
    return 0;
}
