#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    const char* sock_path = (argc > 1) ? argv[1] : "/tmp/zerobus_echo.sock";
    const char* msg       = (argc > 2) ? argv[2] : "hola desde cliente\n";

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return 1; }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path)-1);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(fd);
        return 1;
    }

    if (write(fd, msg, strlen(msg)) < 0) { perror("write"); close(fd); return 1; }

    char buf[512] = {0};
    ssize_t n = read(fd, buf, sizeof(buf)-1);
    if (n < 0) { perror("read"); close(fd); return 1; }

    printf("[client] recv: %s", buf);
    close(fd);
    return 0;
}
