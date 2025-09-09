#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static volatile sig_atomic_t running = 1;
void handle_sigint(int sig){ (void)sig; running = 0; }

int main(int argc, char** argv) {
    const char* sock_path = (argc > 1) ? argv[1] : "/tmp/zerobus_echo.sock";
    signal(SIGINT, handle_sigint);

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return 1; }

    // limpia socket anterior si existe
    unlink(sock_path);

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path)-1);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(fd);
        return 1;
    }
    if (listen(fd, 4) < 0) { perror("listen"); close(fd); return 1; }

    printf("[server] listening on %s (Ctrl+C para salir)\n", sock_path);

    while (running) {
        int cfd = accept(fd, NULL, NULL);
        if (cfd < 0) {
            if (errno == EINTR) break;
            perror("accept"); continue;
        }
        // eco línea a línea
        char buf[512];
        ssize_t n;
        while ((n = read(cfd, buf, sizeof(buf))) > 0) {
            // respuesta en MAYÚSCULAS
            for (ssize_t i = 0; i < n; ++i)
                if (buf[i] >= 'a' && buf[i] <= 'z') buf[i] -= 32;
            if (write(cfd, buf, n) < 0) { perror("write"); break; }
        }
        close(cfd);
    }

    close(fd);
    unlink(sock_path);
    printf("[server] bye\n");
    return 0;
}
