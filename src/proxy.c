/*
 *  Simple HTTP proxy implementation.
 */
#include "proxy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

struct http_proxy {
    int                 socket_fd;
    struct sockaddr_in  addr;
    unsigned int        port;
};

struct http_proxy_client {
    int                 socket_fd;
    struct sockaddr_in  addr;
    socklen_t           socklen;
};

int proxy_init(http_proxy **proxy, unsigned int port) {\
    int error = 0;

    if (proxy == NULL) return 1;

    *proxy = malloc(sizeof(http_proxy));
    memset(*proxy, 0, sizeof(http_proxy));

    (*proxy)->socket_fd =
        socket(AF_INET, SOCK_STREAM | O_NONBLOCK, IPPROTO_TCP);
    if ((*proxy)->socket_fd <= 0) goto err;
    
    (*proxy)->addr.sin_family = AF_INET;
    (*proxy)->addr.sin_port = htons(port);
    (*proxy)->addr.sin_addr.s_addr = htonl(INADDR_ANY);

    error = bind((*proxy)->socket_fd,
                 (struct sockaddr *)&((*proxy)->addr),
                 sizeof((*proxy)->addr));
    if (error == -1) goto err;

    return 0;
err:
    free((*proxy));
    *proxy = NULL;
    return 1;
}

int proxy_listen(http_proxy *proxy, unsigned int backlog) {
    int error = 0;

    if (proxy == NULL) return 1;
    if (backlog < 1) return 1;

    error = listen(proxy->socket_fd, backlog);

    return error;
}

http_proxy_client *proxy_accept(http_proxy *proxy) {
    int error = 0;
    http_proxy_client *client;

    if (proxy == NULL) return NULL;

    client = malloc(sizeof(http_proxy_client));
    memset(client, 0, sizeof(http_proxy_client));

    client->socklen = sizeof(client->addr);

    client->socket_fd = accept(proxy->socket_fd,
                               (struct sockaddr *)&(client->addr),
                               &(client->socklen));
    if (client->socket_fd <= 0) goto err;

    return client;
err:
    free(client);
    return NULL;
}

int proxy_shutdown(http_proxy *proxy) {
    return 0;
}

pid_t start_worker(worker_job *job) {
    pid_t worker_pid;
    if (job == NULL) return -1;

    if (!(worker_pid = fork())) {
        job();
        _exit(0);
    }

    return worker_pid;
}