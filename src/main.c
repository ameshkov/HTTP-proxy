/*
 *  Simple HTTP proxy entry point.
 */
#include "proxy.h"

#include <stdio.h>
#include <stdlib.h>

#define PROXY_PORT_DEFAULT      8888
#define PROXY_CLIENT_QUEUE_MAX  32

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

    while (1) {
        error = proxy_listen(proxy, PROXY_CLIENT_QUEUE_MAX);
        if (error) return 1;

        client = proxy_accept(proxy);
        if (!client) return 1;
    }

    return 0;
}
