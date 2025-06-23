// imsg_socketpair.c
// Compile with: cc -o imsg_socketpair imsg_socketpair.c -lutil

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "imsg.h"

#define OUTFILE "/tmp/received.dat"
#define MSG_TYPE 1

int main(void) {
    int sv[2];
    pid_t pid;
    struct imsgbuf ibuf;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
        perror("socketpair");
        exit(1);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // Child: sender via imsg
        close(sv[0]);
        imsgbuf_init(&ibuf, sv[1]);
        const char *msg = "Hello from imsg over socketpair!";
        imsg_compose(&ibuf, MSG_TYPE, 0, 0, -1,
                     msg, strlen(msg));
        if (imsgbuf_flush(&ibuf) == -1) {
            perror("imsg_flush");
            exit(1);
        }
        close(sv[1]);
        exit(0);
    } else {
        // Parent: raw read + write to file
        close(sv[1]);
        int out_fd = open(OUTFILE, O_CREAT|O_WRONLY|O_TRUNC, 0644);
        if (out_fd == -1) {
            perror("open output file");
            exit(1);
        }

        char buf[4096];
        ssize_t n;
        while ((n = read(sv[0], buf, sizeof(buf))) > 0) {
            if (write(out_fd, buf, n) != n) {
                perror("write");
                break;
            }
        }
        if (n == -1)
            perror("read");

        close(out_fd);
        close(sv[0]);
        wait(NULL);
    }

    return 0;
}
