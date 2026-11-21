// Task 9: Fiber creation with ucontext.h and a user-allocated stack
#include <ucontext.h>
#include <cstdio>
#include <cstdlib>

ucontext_t ctx_main, ctx_worker;
const int FIBER_STACK_SIZE = 16384;     // 16 KB

void fiber_entry() {
    printf("Inside fiber function execution.\n");
    printf("Switching back to main program...\n");
    // returning here automatically switches to uc_link (= ctx_main)
}

int main() {
    getcontext(&ctx_worker);
    ctx_worker.uc_stack.ss_sp = malloc(FIBER_STACK_SIZE);
    ctx_worker.uc_stack.ss_size = FIBER_STACK_SIZE;
    ctx_worker.uc_link = &ctx_main;     // where to go when fiber_entry returns

    makecontext(&ctx_worker, fiber_entry, 0);

    printf("Yielding execution to fiber...\n");
    swapcontext(&ctx_main, &ctx_worker);
    printf("Returned cleanly to main process.\n");

    free(ctx_worker.uc_stack.ss_sp);
    return 0;
}
