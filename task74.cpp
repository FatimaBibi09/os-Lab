// Task 19: Hybrid M:N runtime - 10,000 user-level fibers on 4 kernel worker threads
#include <pthread.h>
#include <ucontext.h>
#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <queue>
#include <unistd.h>

const int M_WORKERS = 4;                // 4 OS kernel threads
const int N_TASKS = 10000;              // 10,000 lightweight fibers
const size_t FIBER_STACK = 16 * 1024;

struct ULT_Task {
    int id;
    ucontext_t ctx;
    char* stack = nullptr;
    ucontext_t* return_ctx = nullptr;   // worker context to go back to
    bool finished = false;
    bool shutdown = false;
};

struct ThreadSafeQueue {
    std::queue<ULT_Task*> q;
    std::mutex mtx;
    std::condition_variable cv;
    void push(ULT_Task* t) { { std::lock_guard<std::mutex> l(mtx); q.push(t); } cv.notify_one(); }
    ULT_Task* pop() {                   // blocks safely on mutex / condvar
        std::unique_lock<std::mutex> l(mtx);
        cv.wait(l, [&] { return !q.empty(); });
        ULT_Task* t = q.front(); q.pop();
        return t;
    }
};

ThreadSafeQueue task_queue;
std::atomic<int> completed(0);
std::atomic<long long> total(0);
std::atomic<int> per_worker[M_WORKERS];
std::atomic<int> migrated(0);

thread_local ULT_Task* running_task = nullptr;   // only read BEFORE a switch, never cached across one

void task_routine(ULT_Task* me, int j) {
    long long partial = 0;
    for (int i = 1; i <= 1000; i++) partial += i % (j + 1);
    // yield halfway: the fiber goes back to the queue and may resume on ANOTHER worker
    swapcontext(&me->ctx, me->return_ctx);
    for (int i = 1; i <= 1000; i++) partial += i % (j + 1);
    total += partial;
}

void fiber_entry_wrapper() {
    ULT_Task* me = running_task;            // read once, before any switch
    task_routine(me, me->id);
    me->finished = true;
    // We do NOT rely on uc_link: makecontext() bakes it in at creation time, but this
    // fiber may now be running on a different worker. Switch back explicitly instead.
    swapcontext(&me->ctx, me->return_ctx);
}

void* kernel_worker_loop(void* arg) {
    int worker_id = (int)(long)arg;
    ucontext_t worker_ctx;
    while (true) {
        ULT_Task* task = task_queue.pop();
        if (task->shutdown) { delete task; break; }

        if (!task->stack) {             // first time: build the fiber
            task->stack = (char*)malloc(FIBER_STACK);
            getcontext(&task->ctx);
            task->ctx.uc_stack.ss_sp = task->stack;
            task->ctx.uc_stack.ss_size = FIBER_STACK;
            task->ctx.uc_link = nullptr;
            makecontext(&task->ctx, fiber_entry_wrapper, 0);
        } else {
            migrated++;                         // resumed, maybe on a different worker
        }
        task->return_ctx = &worker_ctx;
        running_task = task;
        swapcontext(&worker_ctx, &task->ctx);       // execute_user_fiber
        per_worker[worker_id]++;

        // fiber either finished (stack no longer needed) or yielded (re-queue it)
        if (task->finished) { free(task->stack); delete task; completed++; }  // finished
        else task_queue.push(task);                                           // yielded: re-queue
    }
    return NULL;
}

int main() {
    pthread_t workers[M_WORKERS];
    for (long i = 0; i < M_WORKERS; i++)
        pthread_create(&workers[i], NULL, kernel_worker_loop, (void*)i);

    for (int j = 1; j <= N_TASKS; j++) {
        ULT_Task* t = new ULT_Task;
        t->id = j;
        task_queue.push(t);
    }

    while (completed < N_TASKS) usleep(10000);
    for (int i = 0; i < M_WORKERS; i++) {
        ULT_Task* s = new ULT_Task; s->shutdown = true; task_queue.push(s);
    }
    for (int i = 0; i < M_WORKERS; i++) pthread_join(workers[i], NULL);

    printf("Completed %d fibers on %d kernel threads (checksum %lld)\n", completed.load(), M_WORKERS, total.load());
    for (int i = 0; i < M_WORKERS; i++) printf("  worker %d ran %d fiber slices\n", i, per_worker[i].load());
    printf("Fibers resumed on a worker after yielding: %d\n", migrated.load());
    return 0;
}
