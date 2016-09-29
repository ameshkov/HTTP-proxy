/*
 *  Simple HTTP proxy API header.
 */
#ifndef PROXY_H
#define PROXY_H

typedef struct http_proxy http_proxy;

int proxy_init(http_proxy **proxy, unsigned int port);

int proxy_listen(http_proxy *proxy);

int proxy_shutdown(http_proxy *proxy);

#endif /* PROXY_H */