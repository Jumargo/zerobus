#define _GNU_SOURCE
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static volatile sig_atomic_t running = 1;
void handle_sigint(int sig){ (void)sig; running = 0; }

int main(void) {
    signal(SIGINT, handle_sigint);

    int p2c[2]; // parent -> child
    int c2p[2]; // child  -> parent
    if (pipe(p2c) == -1 || pipe(c2p) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        // --- CHILD ---
        close(p2c[1]); // child no escribe en p2c
        close(c2p[0]); // child no lee de c2p

        char buf[256];
        ssize_t n;
        while ((n = read(p2c[0], buf, sizeof(buf)-1)) > 0) {
            buf[n] = '\0';
            // responde en mayúsculas
            for (ssize_t i = 0; i < n; ++i) if ('a' <= buf[i] && buf[i] <= 'z') buf[i] -= 32;
            dprintf(c2p[1], "child echo: %s", buf);
        }
        close(p2c[0]);
        close(c2p[1]);
        _exit(0);
    }

    // --- PARENT ---
    close(p2c[0]); // parent no lee de p2c
    close(c2p[1]); // parent no escribe en c2p

    const char *msgs[] = {"hola\n", "pipe\n", "linux\n", NULL};
    for (int i = 0; msgs[i] && running; ++i) {
        dprintf(p2c[1], "%s", msgs[i]);
        char rbuf[256] = {0};
        ssize_t n = read(c2p[0], rbuf, sizeof(rbuf)-1);
        if (n > 0) {
            rbuf[n] = '\0';
            printf("[parent] recv: %s", rbuf);
        }
        usleep(200*1000);
    }

    close(p2c[1]);
    close(c2p[0]);

    int status = 0;
    waitpid(pid, &status, 0);
    printf("[parent] child exited with %d\n", WEXITSTATUS(status));
    return 0;
}
