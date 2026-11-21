// Task 4: Multi-core parallelism and CPU pinning (pthread_setaffinity_np)
// Usage: ./task04 [run_seconds]  (default 5). Watch with `top` then press 1.
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

std::atomic<bool> stop_flag(false);
long long iterations[256];

void* compute_heavy_work(void* arg) {
    int id = (int)(long)arg;
    volatile double result = 0;
    long long n = 0;
    while (!stop_flag) {            // keep the core at 100%
        result = sqrt((double)rand()) * sin((double)rand());
        n++;
    }
    iterations[id] = n;
    printf("Thread %d finished on CPU %d after %lld loops\n", id, sched_getcpu(), n);
    return NULL;
}

int main(int argc, char** argv) {
    int run_secs = (argc > 1) ? atoi(argv[1]) : 5;
    int num_cores = (int)sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cores > 256) num_cores = 256;
    printf("Detected %d CPU cores. Running %d seconds...\n", num_cores, run_secs);

    std::vector<pthread_t> threads(num_cores);
    for (int i = 0; i < num_cores; i++) {
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        CPU_SET(i, &cpu_set);
        pthread_create(&threads[i], NULL, compute_heavy_work, (void*)(long)i);
        pthread_setaffinity_np(threads[i], sizeof(cpu_set), &cpu_set);
    }
    sleep(run_secs);                // observe saturation in `top`
    stop_flag = true;
    for (int i = 0; i < num_cores; i++) pthread_join(threads[i], NULL);
    return 0;
}
