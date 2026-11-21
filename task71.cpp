// Task 16: Context-switch latency: ULT (swapcontext) vs KLT (pipe ping-pong)
// Usage: ./task16 [switch_count]   (default 1000000; use less if slow)
// Note: glibc's swapcontext also does a sigprocmask syscall, so it is slower than
// a hand-written assembly switch (the "5-30 ns" in the table).
#include <ucontext.h>
#include <pthread.h>
#include <unistd.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>

long SWITCH_COUNT = 1000000;
typedef std::chrono::steady_clock Clock;

ucontext_t ctx1, ctx2;

void fiber_fn() {
    for (;;) swapcontext(&ctx2, &ctx1);
}

void benchmark_ult() {
    getcontext(&ctx2);
    ctx2.uc_stack.ss_sp = malloc(65536);
    ctx2.uc_stack.ss_size = 65536;
    ctx2.uc_link = &ctx1;
    makecontext(&ctx2, fiber_fn, 0);

    auto start = Clock::now();
    for (long i = 0; i < SWITCH_COUNT; i++)
        swapcontext(&ctx1, &ctx2);                      // 1 round trip = 2 switches
    double ns = std::chrono::duration<double, std::nano>(Clock::now() - start).count();
    printf("Avg ULT Switch Latency: %.1f ns\n", ns / (2.0 * SWITCH_COUNT));
}

int ping[2], pong[2];

void* pong_thread(void*) {
    char c;
    for (long i = 0; i < SWITCH_COUNT; i++) {
        read(ping[0], &c, 1);
        write(pong[1], &c, 1);
    }
    return NULL;
}

void benchmark_klt() {
    pipe(ping);
    pipe(pong);
    pthread_t t;
    pthread_create(&t, NULL, pong_thread, NULL);
    char c = 'x';
    auto start = Clock::now();
    for (long i = 0; i < SWITCH_COUNT; i++) {
        write(ping[1], &c, 1);
        read(pong[0], &c, 1);
    }
    double ns = std::chrono::duration<double, std::nano>(Clock::now() - start).count();
    pthread_join(t, NULL);
    printf("Avg KLT Switch Latency: %.1f ns (includes pipe system-call cost)\n", ns / (2.0 * SWITCH_COUNT));
}

int main(int argc, char** argv) {
    if (argc > 1) SWITCH_COUNT = atol(argv[1]);
    printf("Switch count: %ld round trips\n", SWITCH_COUNT);
    benchmark_ult();
    benchmark_klt();
    return 0;
}
