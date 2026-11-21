// Task 7: Thread-specific data - compiler TLS (thread_local) and POSIX keys
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

thread_local int private_counter = 0;     // every thread gets its own copy
pthread_key_t tls_key;

void* worker(void* arg) {
    int id = (int)(long)arg;
    private_counter += id;

    char* buffer = (char*)malloc(256);
    snprintf(buffer, 256, "Log entry for thread %d", id);
    pthread_setspecific(tls_key, buffer);

    char* retrieved_buf = (char*)pthread_getspecific(tls_key);
    printf("Thread: %d TLS Data: %s Private: %d\n", id, retrieved_buf, private_counter);
    free(retrieved_buf);
    return NULL;
}

int main() {
    pthread_key_create(&tls_key, NULL);
    pthread_t t[4];
    for (long i = 0; i < 4; i++) pthread_create(&t[i], NULL, worker, (void*)(i + 1));
    for (int i = 0; i < 4; i++) pthread_join(t[i], NULL);
    pthread_key_delete(tls_key);
    printf("main's private_counter is still %d (untouched by workers)\n", private_counter);
    return 0;
}
