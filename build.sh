#!/bin/bash

[ -x bin/proxy ] && rm bin/proxy

gcc -o bin/proxy \
    src/proxy.h \
    src/proxy.c \
    src/main.c

[ $? -eq 0 ] && bin/proxy
