
TARGET = proxy

all: $(TARGET)

proxy: src/main.c src/proxy.c src/proxy.h deps/lib/libhttpp.a deps/lib/libz.a
	gcc -I deps/include -o bin/proxy src/main.c src/proxy.c src/proxy.h deps/lib/libhttpp.a deps/lib/libz.a
