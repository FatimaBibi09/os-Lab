// Task 15: Non-blocking I/O for user threads using epoll + the ULT scheduler
// Type something and press Enter. The heartbeat keeps ticking while waiting.
#include "ult.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

int epfd;
bool io_done = false;

ssize_t async_user_read(int fd, char* buffer, size_t length) {
    int flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);             // set_nonblocking_flag
    ssize_t bytes = read(fd, buffer, length);
    while (bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        epoll_event ev = {};
        ev.events = EPOLLIN | EPOLLONESHOT;
        ev.data.ptr = current_thread;
        if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) < 0)
            epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);    // register_with_epoll
        current_thread->state = WAITING_IO;
        swapcontext(&current_thread->context, &scheduler_context);
        // resumed once epoll reports the fd is readable
        bytes = read(fd, buffer, length);
    }
    return bytes;
}

void io_thread(void*) {
    printf("ULT 1: waiting for input (type text + Enter)...\n");
    fflush(stdout);
    char buf[100];
    ssize_t n = async_user_read(STDIN_FILENO, buf, sizeof buf - 1);
    if (n < 0) n = 0;
    buf[n] = 0;
    printf("ULT 1: Received %zd bytes: %s", n, buf);
    if (n == 0 || buf[n - 1] != '\n') printf("\n");
    io_done = true;
}

void heartbeat(void*) {
    int tick = 0;
    while (!io_done) {
        printf("ULT 2: Heartbeat tick %d (still running while ULT 1 waits)\n", ++tick);
        fflush(stdout);
        usleep(500000);
        my_thread_yield();
    }
}

// Called by scheduler_loop() on every iteration: poll epoll, wake ready I/O threads
void poll_epoll_events() {
    epoll_event evs[8];
    int timeout = ready_queue.empty() ? 100 : 0;        // sleep a little only if nothing to run
    int n = epoll_wait(epfd, evs, 8, timeout);
    for (int i = 0; i < n; i++) {
        ULT_TCB* t = (ULT_TCB*)evs[i].data.ptr;
        t->state = READY;
        ready_queue.push(t);                            // wake_ready_io_threads
    }
}

int main() {
    epfd = epoll_create1(0);
    scheduler_idle_hook = poll_epoll_events;
    ult_create(io_thread, NULL);
    ult_create(heartbeat, NULL);
    scheduler_loop();
    close(epfd);
    return 0;
}
