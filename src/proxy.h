/*
 *  Simple HTTP proxy API header.
 */
#ifndef PROXY_H
#define PROXY_H

#include <unistd.h>

typedef struct http_proxy http_proxy;

typedef struct http_proxy_client http_proxy_client;

typedef void worker_job(void);

int proxy_init(http_proxy **proxy, unsigned int port);

int proxy_listen(http_proxy *proxy, unsigned int backlog);

http_proxy_client *proxy_accept(http_proxy *proxy);

int proxy_shutdown(http_proxy *proxy);

pid_t start_worker(worker_job *job);

#endif /* PROXY_H */