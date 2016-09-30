/*
 *  Simple HTTP proxy entry point.
 */
#include "proxy.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#define PROXY_PORT_DEFAULT      8888
#define PROXY_CLIENT_QUEUE_MAX  32
#define NUM_WORKERS             3

static pid_t           workers[NUM_WORKERS];
static unsigned int    active_workers = 0;

void child_exit_handler(int signo) {
    signal (SIGCHLD, child_exit_handler);
    wait(NULL);
}

void process_clients(void) {
    //while (1);
}

void kill_workers(void) {
    int status;
    for (int i = 0; i < active_workers; i++) {
        kill(workers[i], SIGTERM);
    }
    active_workers = 0;
}

int run_workers(unsigned int num_workers, worker_job *job) {
    pid_t worker_pid;
    if (num_workers == 0 || num_workers > NUM_WORKERS || job == NULL) return 0;
    if (active_workers != 0) return 0;

    for (int i = 0; i < num_workers; i++) {
        worker_pid = start_worker((worker_job *)job);
        if (worker_pid <= 0) {
            kill_workers();
            return 0;
        }
        workers[i] = worker_pid;
        active_workers++;
    }

    return active_workers;
}

int main(int argc, char** argv)
{
    http_proxy *proxy;
    http_proxy_client *client;
    unsigned int port = PROXY_PORT_DEFAULT;
    int error = 0;
    
    if (argc == 2) port = atoi(argv[1]);
    if (port == 0) return 1;

    error = proxy_init(&proxy, port);
    if (error) return 1;

    signal (SIGCHLD, child_exit_handler);

    run_workers(NUM_WORKERS, (worker_job *)process_clients);

    while (1) {
        error = proxy_listen(proxy, PROXY_CLIENT_QUEUE_MAX);
        if (error) return 1;

        //client = proxy_accept(proxy);
        //if (!client) return 1;
    }

    return 0;
}
