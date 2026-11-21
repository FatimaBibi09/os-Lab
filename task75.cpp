// Task 20: Signal routing: one dedicated KLT handles SIGINT/SIGTERM via sigwait
// The worker sends signals to the process itself so the demo is automatic.
// You can also press Ctrl+C while it runs. SIGTERM ends the program.
#include <pthread.h>
#include <csignal>
#include <unistd.h>
#include <cstdio>

void* dedicated_signal_handler_thread(void*) {
    sigset_t monitored_signals;
    sigemptyset(&monitored_signals);
    sigaddset(&monitored_signals, SIGINT);
    sigaddset(&monitored_signals, SIGTERM);
    int received_signal = 0;

    while (true) {
        sigwait(&monitored_signals, &received_signal);   // synchronously await delivery
        printf("Signal received cleanly by designated KLT: %d (%s)\n", received_signal,
               received_signal == SIGINT ? "SIGINT" : "SIGTERM");
        fflush(stdout);
        if (received_signal == SIGTERM) break;
    }
    return NULL;
}

void* regular_worker_routine(void*) {
    for (int i = 1; i <= 2; i++) {
        printf("Worker: doing work... sending SIGINT to the process (#%d)\n", i);
        fflush(stdout);
        kill(getpid(), SIGINT);
        sleep(1);
    }
    printf("Worker: sending SIGTERM to the process\n");
    fflush(stdout);
    kill(getpid(), SIGTERM);
    return NULL;
}

int main() {
    // Block the signals in the main thread; threads created afterwards inherit the mask,
    // so ONLY sigwait() in the dedicated thread ever receives them.
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    pthread_t sig_thread, worker_thread;
    pthread_create(&sig_thread, NULL, dedicated_signal_handler_thread, NULL);
    pthread_create(&worker_thread, NULL, regular_worker_routine, NULL);

    // Note for ULT: signals arrive at process level and interrupt whichever fiber
    // happens to be running; there is no per-fiber signal mask.
    pthread_join(worker_thread, NULL);
    pthread_join(sig_thread, NULL);
    printf("Program finished cleanly.\n");
    return 0;
}
