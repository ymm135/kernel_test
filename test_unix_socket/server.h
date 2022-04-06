#ifndef __SERVER_H__
#define __SERVER_H__

int init_server(const char *sock);
void run_server(int worker_index, int listen_fd);

#endif  //__SERVER_H__