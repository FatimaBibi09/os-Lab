// Task 11: Preemptive ULT scheduler using setitimer + SIGVTALRM
// The ULTs never call yield; the timer signal forces the switch.
#include "ult.h"
#include <sys/time.h>
#include <unistd.h>

volatile sig_atomic_t stop_flag = 0;
volatile int ticks = 0;
long counters[4];

void timer_signal_handler(int) {
    ticks++;
    char msg[100];
    int n = snprintf(msg, sizeof msg, "[tick %d] Timer expired: preempting ULT %d\n",
                     (int)ticks, current_thread ? current_thread->id : -1);
    write(1, msg, n);
    if (ticks >= 10) {                  // stop after 10 ticks (200 ms of CPU time)
        stop_flag = 1;
        itimerval off = {};
        setitimer(ITIMER_VIRTUAL, &off, NULL);
    }
    my_thread_yield();                  // interrupt current ULT on timer tick
}

void init_preemption() {
    struct sigaction sa = {};
    sa.sa_handler = timer_signal_handler;
    sigaction(SIGVTALRM, &sa, NULL);

    itimerval timer_conf = {};
    timer_conf.it_interval.tv_usec = 20000;   // every 20 ms
    timer_conf.it_value.tv_usec = 20000;
    setitimer(ITIMER_VIRTUAL, &timer_conf, NULL);
}

void compute_infinite(void* arg) {
    int id = (int)(long)arg;
    while (!stop_flag) {                // NO explicit yields in this loop
        counters[id]++;
    }
}

int main() {
    for (long i = 1; i <= 3; i++) ult_create(compute_infinite, (void*)i);
    init_preemption();
    scheduler_loop();
    for (int i = 1; i <= 3; i++)
        printf("ULT %d did %ld loop iterations (it got CPU time without yielding)\n", i, counters[i]);
    return 0;
}
