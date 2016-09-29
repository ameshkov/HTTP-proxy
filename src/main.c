/*
 *  Simple HTTP proxy entry point.
 */
#include "proxy.h"

#include <stdio.h>
#include <stdlib.h>

#define PROXY_PORT_DEFAULT  8888

int main(int argc, char** argv)
{
    http_proxy *proxy;
    unsigned int port = PROXY_PORT_DEFAULT;
    int error = 0;
    
    if (argc == 2) port = atoi(argv[1]);
    if (port == 0) return 1;

    error = proxy_init(&proxy, port);
    if (error) return 1;


    return 0;
}
