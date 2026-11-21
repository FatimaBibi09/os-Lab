// Task 18: CPU-bound matrix multiply: 4 ULTs in 1 kernel thread vs 4 pthreads
// Usage: ./task18 [matrix_dim]   (default 500)
#include "ult.h"
#include <pthread.h>
#include <chrono>
#include <thread>
#include <vector>

int MATRIX_DIM = 500;
std::vector<double> A, B, C;
typedef std::chrono::steady_clock Clock;

struct Range { int start_row, end_row; };

void compute_dot_product(int i, int j) {
    double sum = 0;
    for (int k = 0; k < MATRIX_DIM; k++) sum += A[i * MATRIX_DIM + k] * B[k * MATRIX_DIM + j];
    C[i * MATRIX_DIM + j] = sum;
}

void matrix_multiply_subsegment(int start_row, int end_row) {
    for (int i = start_row; i < end_row; i++)
        for (int j = 0; j < MATRIX_DIM; j++)
            compute_dot_product(i, j);
}

void ult_task(void* arg) {
    Range* r = (Range*)arg;
    for (int i = r->start_row; i < r->end_row; i++) {
        matrix_multiply_subsegment(i, i + 1);
        my_thread_yield();                  // cooperative: one row at a time
    }
}

void* pthread_task(void* arg) {
    Range* r = (Range*)arg;
    matrix_multiply_subsegment(r->start_row, r->end_row);
    return NULL;
}

double checksum() { double s = 0; for (double v : C) s += v; return s; }

int main(int argc, char** argv) {
    if (argc > 1) MATRIX_DIM = atoi(argv[1]);
    A.assign((size_t)MATRIX_DIM * MATRIX_DIM, 1.5);
    B.assign((size_t)MATRIX_DIM * MATRIX_DIM, 2.0);
    C.assign((size_t)MATRIX_DIM * MATRIX_DIM, 0.0);
    printf("Matrix %dx%d, CPU cores available: %u\n", MATRIX_DIM, MATRIX_DIM,
           std::thread::hardware_concurrency());

    int q = MATRIX_DIM / 4;
    Range ranges[4] = {{0, q}, {q, 2 * q}, {2 * q, 3 * q}, {3 * q, MATRIX_DIM}};

    // Test Case A: 4 ULTs inside 1 kernel thread
    auto t0 = Clock::now();
    for (int i = 0; i < 4; i++) ult_create(ult_task, &ranges[i]);
    scheduler_loop();
    double tA = std::chrono::duration<double>(Clock::now() - t0).count();
    printf("ULT Completion Wall-Clock: %.3f s (checksum %.0f)\n", tA, checksum());

    std::fill(C.begin(), C.end(), 0.0);

    // Test Case B: 4 kernel threads spread over the cores
    t0 = Clock::now();
    pthread_t th[4];
    for (int i = 0; i < 4; i++) pthread_create(&th[i], NULL, pthread_task, &ranges[i]);
    for (int i = 0; i < 4; i++) pthread_join(th[i], NULL);
    double tB = std::chrono::duration<double>(Clock::now() - t0).count();
    printf("KLT Completion Wall-Clock: %.3f s (checksum %.0f)\n", tB, checksum());
    printf("Speedup (ULT time / KLT time): %.2fx\n", tA / tB);
    return 0;
}
