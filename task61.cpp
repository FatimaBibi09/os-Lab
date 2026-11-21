// Task 6: Bounded buffer (producer-consumer) with condition variables
#include <pthread.h>
#include <cstdio>

const int CAPACITY = 5;
int buffer[CAPACITY];
int count = 0, in_idx = 0, out_idx = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void producer_enqueue(int item) {
    pthread_mutex_lock(&mutex);
    while (count == CAPACITY)                       // while (not if) handles spurious wakeups
        pthread_cond_wait(&not_full, &mutex);       // atomically releases mutex and sleeps
    buffer[in_idx] = item;
    in_idx = (in_idx + 1) % CAPACITY;
    count++;
    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&mutex);
}

int consumer_dequeue() {
    pthread_mutex_lock(&mutex);
    while (count == 0)
        pthread_cond_wait(&not_empty, &mutex);
    int item = buffer[out_idx];
    out_idx = (out_idx + 1) % CAPACITY;
    count--;
    pthread_cond_signal(&not_full);
    pthread_mutex_unlock(&mutex);
    return item;
}

void* producer(void* arg) {
    long id = (long)arg;
    for (int i = 0; i < 10; i++) {
        int item = (int)id * 100 + i;
        producer_enqueue(item);
        printf("Producer %ld produced %d\n", id, item);
    }
    return NULL;
}

void* consumer(void* arg) {
    long id = (long)arg;
    for (int i = 0; i < 10; i++) {
        int item = consumer_dequeue();
        printf("    Consumer %ld consumed %d\n", id, item);
    }
    return NULL;
}

int main() {
    pthread_t p[2], c[2];
    for (long i = 0; i < 2; i++) {
        pthread_create(&p[i], NULL, producer, (void*)(i + 1));
        pthread_create(&c[i], NULL, consumer, (void*)(i + 1));
    }
    for (int i = 0; i < 2; i++) { pthread_join(p[i], NULL); pthread_join(c[i], NULL); }
    printf("Done. Items left in buffer: %d (should be 0)\n", count);
    return 0;
}
