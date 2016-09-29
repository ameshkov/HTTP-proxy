/*
 *  Simple HTTP proxy implementation.
 */
#include "proxy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct http_proxy {
    int                 socket_fd;
    struct sockaddr     addr;
    unsigned int        port;
};

int proxy_init(http_proxy **proxy, unsigned int port) {
    if (proxy == NULL) return 1;

    *proxy = malloc(sizeof(http_proxy));
    memset(*proxy, 0, sizeof(http_proxy));

    (*proxy)->socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if ((*proxy)->socket_fd <= 0) goto err;
    

    return 0;
err:
    free((*proxy));
    *proxy = NULL;
    return 1;
}

int proxy_listen(http_proxy *proxy) {
    return 0;
}

int proxy_shutdown(http_proxy *proxy) {
    return 0;
}