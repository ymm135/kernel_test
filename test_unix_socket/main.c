/* author: wenfh2020/2021.06.18
 * desc:   test tcp thundering herd in epoll.
 * ver:    Linux 5.0.1
 * test:   make rootfs --> input 's' --> input 'c' */

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"

#include "server.h"
#include "client.h"

#define SERVER_PATH     "/tmp/server"
#define SEND_DATA "hello."

#define WORKER_CNT 1 

/* fork workers to listen. */
int workers(int worker_cnt, const char *sock);

int main(int argc, char **argv) {
    char buf[64] = {0};

    LOG("pls input 's' to run server or 'c' to run client!");

    while (1) {
        scanf("%s", buf);

        if (strcmp(buf, "s") == 0) {
            proc(SERVER_PATH);
        } else if (strcmp(buf, "c") == 0) {
            proc_unix_client(SERVER_PATH, SEND_DATA);
        } else {
            LOG("pls input 's' to run server or 'c' to run client!");
        }
    }

    return 0;
}

void proc(const char *sock) {
    int pid;

    pid = fork();
    if (pid == 0) {
        /* child */
        workers(WORKER_CNT, sock);
    } else if (pid > 0) {
        /* parent */
        LOG("pls input 'c' to run client!");
    } else {
        /* error */
        LOG_SYS_ERR("fork failed!");
        exit(-1);
    }
}

int workers(int worker_cnt, const char *sock) {
    LOG("unix workers...");

    int i, fd, pid;

    fd = init_server(sock);
    if (fd < 0) {
        LOG("init server failed!");
        return 0;
    }

    for (i = 0; i < worker_cnt; i++) {
        pid = fork();
        if (pid == 0) {
            /* child. */
            run_server(i, fd);
        } else if (pid > 0) {
            /* parent */
            LOG("fork child, pid: %d", pid);
        } else {
            /* error */
            LOG_SYS_ERR("fork failed!");
            exit(-1);
        }
    }

    return 1;
}
