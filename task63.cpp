// Task 8: Cooperative context switching with setjmp / longjmp
//
// Note: longjmp may only jump to a function that is STILL ACTIVE on the stack.
// So a scheduler in main() calls setjmp once, and each task does one step and
// longjmp()s back to it. Task state (the step counters) lives in globals.
#include <csetjmp>
#include <cstdio>

jmp_buf sched_ctx;
int step_A = 0, step_B = 0;
int turn = 0;                           // global: locals are unsafe across longjmp
const int STEPS = 5;

void task_A() {
    step_A++;
    printf("Task A executing step: %d\n", step_A);
    longjmp(sched_ctx, 1);              // switch back to scheduler (no system call!)
}

void task_B() {
    step_B++;
    printf("Task B executing step: %d\n", step_B);
    longjmp(sched_ctx, 2);
}

int main() {
    int from = setjmp(sched_ctx);       // execution comes back here after every longjmp
    if (from != 0)
        printf("   [scheduler] back from task %c\n", from == 1 ? 'A' : 'B');

    if (step_A < STEPS || step_B < STEPS) {
        turn++;
        bool want_A = (turn % 2 == 1);          // alternate A, B, A, B ...
        if (want_A && step_A >= STEPS) want_A = false;
        if (!want_A && step_B >= STEPS) want_A = true;
        if (want_A) task_A(); else task_B();
    }
    printf("Both tasks finished.\n");
    return 0;
}
