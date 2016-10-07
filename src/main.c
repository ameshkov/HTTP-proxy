/*
 *  Simple HTTP proxy entry point.
 */
#include "proxy.h"

#include <parser.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>

#define PROXY_PORT_DEFAULT      8888
#define PROXY_CLIENT_QUEUE_MAX  32
#define NUM_WORKERS             3
#define RECEIVE_BUFFER_SIZE     64
#define SELECT_TIMEOUT_USEC     500

static pid_t           workers[NUM_WORKERS];
static unsigned int    active_workers = 0;

fd_set master_fds, read_fds, write_fds;

struct timeval select_to = {.tv_sec=0,.tv_usec=SELECT_TIMEOUT_USEC};

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
    unsigned int max_fd;
    int error = 0, sel_res, nbytes;
    char recv_buff[RECEIVE_BUFFER_SIZE];
    
    if (argc == 2) port = atoi(argv[1]);
    if (port == 0) return 1;

    error = proxy_init(&proxy, port);
    if (error) return 1;

    signal (SIGCHLD, child_exit_handler);

    run_workers(NUM_WORKERS, (worker_job *)process_clients);

    FD_ZERO(&master_fds);
    FD_SET(proxy->socket_fd, &master_fds);
    max_fd = proxy->socket_fd;

    while (1) {
        error = proxy_listen(proxy, PROXY_CLIENT_QUEUE_MAX);
        if (error) return 1;
        
        read_fds = write_fds = master_fds;
        sel_res = select(max_fd + 1, &read_fds, &write_fds, NULL, &select_to);
        if (sel_res = 0) continue;
        else  if (sel_res < 0) {
            printf("FATAL ERROR! : Cannot select from fd set: %d.\n", errno);
            return 1;
        }

        for (int i = 0; i < max_fd + 1; i++) {
            if FD_ISSET(i, &read_fds) {
                if (i == proxy->socket_fd) {
                    client = proxy_accept(proxy);
                    if (!client) {
                        printf ("ERROR! : Accept failed!\n");
                        nbytes = recv (i, recv_buff, RECEIVE_BUFFER_SIZE, 0);
                        if (nbytes <= 0) {
                            printf("Close...\n");
                            close(i);
                            FD_CLR(i, &master_fds);
                        } else {
                            recv_buff[nbytes]=0;
                            printf ("%s", recv_buff);
                        }
                    } else {
                        FD_SET(client->socket_fd, &master_fds);
                        if (client->socket_fd > max_fd) max_fd = client->socket_fd;
                        break;
                    }
                } else {
                    nbytes = recv (i, recv_buff, RECEIVE_BUFFER_SIZE, 0);
                    if (nbytes <= 0) {
                        close(i);
                        FD_CLR(i, &master_fds);
                    } else {
                        recv_buff[nbytes]=0;
                        printf ("%s", recv_buff);
                    }
                }
            }
        }

        //client = proxy_accept(proxy);
        //if (!client) return 1;
    }

    return 0;
}
