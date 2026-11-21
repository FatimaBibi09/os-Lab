// Task 5: Futex-based mutex vs spinlock under high contention
#include <pthread.h>
#include <chrono>
#include <cstdio>
#include <vector>

long counter = 0;
pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_spinlock_t spin_lock;

void* thread_increment_mutex(void* arg) {
    long iterations = (long)arg;
    for (long i = 0; i < iterations; i++) {
        pthread_mutex_lock(&mutex_lock);
        counter++;
        pthread_mutex_unlock(&mutex_lock);
    }
    return NULL;
}

void* thread_increment_spin(void* arg) {
    long iterations = (long)arg;
    for (long i = 0; i < iterations; i++) {
        pthread_spin_lock(&spin_lock);
        counter++;
        pthread_spin_unlock(&spin_lock);
    }
    return NULL;
}

double spawn_and_join_threads(int n, void* (*fn)(void*), long iterations) {
    counter = 0;
    auto start = std::chrono::steady_clock::now();
    std::vector<pthread_t> t(n);
    for (int i = 0; i < n; i++) pthread_create(&t[i], NULL, fn, (void*)iterations);
    for (int i = 0; i < n; i++) pthread_join(t[i], NULL);
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

int main() {
    pthread_spin_init(&spin_lock, PTHREAD_PROCESS_PRIVATE);

    double t1 = spawn_and_join_threads(8, thread_increment_mutex, 1000000);
    printf("Mutex    Execution Elapsed: %.3f s (counter = %ld, expected 8000000)\n", t1, counter);

    double t2 = spawn_and_join_threads(8, thread_increment_spin, 1000000);
    printf("Spinlock Execution Elapsed: %.3f s (counter = %ld, expected 8000000)\n", t2, counter);
    return 0;
}
