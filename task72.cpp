// Task 17: How many KLTs vs ULTs can we create?
// Usage: ./task17 [klt_cap] [ult_count]   defaults: 5000  100000
// The manual tries KLTs until failure and 500000 ULTs. Those big numbers can freeze
// a PC, so we use safe caps; raise them yourself if you want (e.g. ./task17 30000 500000).
#include <pthread.h>
#include <unistd.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
bool release_all = false;
typedef std::chrono::steady_clock Clock;

void* dummy_idle_routine(void*) {
    pthread_mutex_lock(&m);
    while (!release_all) pthread_cond_wait(&cv, &m);
    pthread_mutex_unlock(&m);
    return NULL;
}

void stress_test_klt(long cap) {
    std::vector<pthread_t> threads;
    auto start = Clock::now();
    long created = 0;
    while (created < cap) {
        pthread_t t;
        int status = pthread_create(&t, NULL, dummy_idle_routine, NULL);
        if (status != 0) {
            printf("KLT allocation failed at limit: %ld (%s)\n", created, strerror(status));
            break;
        }
        threads.push_back(t);
        created++;
    }
    double s = std::chrono::duration<double>(Clock::now() - start).count();
    printf("Created %ld kernel threads in %.3f s (default stack 8 MB virtual each)\n", created, s);

    pthread_mutex_lock(&m);
    release_all = true;
    pthread_cond_broadcast(&cv);
    pthread_mutex_unlock(&m);
    for (auto& t : threads) pthread_join(t, NULL);
}

struct Fiber { int id; char* stack; };

void stress_test_ult(long count) {
    std::vector<Fiber*> fibers;
    fibers.reserve(count);
    auto start = Clock::now();
    for (long i = 0; i < count; i++) {
        Fiber* f = new Fiber;                 // allocate_ult_descriptor(stack_size = 4096)
        f->id = (int)i;
        f->stack = (char*)malloc(4096);
        fibers.push_back(f);
    }
    double s = std::chrono::duration<double>(Clock::now() - start).count();
    printf("Successfully allocated %ld ULT fibers (4 KB stack) in heap in %.3f s\n", count, s);
    for (auto f : fibers) { free(f->stack); delete f; }
}

int main(int argc, char** argv) {
    long klt_cap = argc > 1 ? atol(argv[1]) : 5000;
    long ult_count = argc > 2 ? atol(argv[2]) : 100000;

    FILE* fp = fopen("/proc/sys/kernel/threads-max", "r");
    if (fp) { long tm; if (fscanf(fp, "%ld", &tm) == 1) printf("Kernel threads-max: %ld\n", tm); fclose(fp); }

    stress_test_klt(klt_cap);
    stress_test_ult(ult_count);
    return 0;
}
