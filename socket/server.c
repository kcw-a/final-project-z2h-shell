#include "stdio.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

typedef enum {
  PROTO_HELLO,
} proto_type_e;

typedef struct {
  proto_type_e type;
  unsigned short len;
} proto_hdr_t;

void handle_client(int cfd) {
  char buf[4096] = {0};
  proto_hdr_t *hdr = (proto_hdr_t *)buf;
  hdr->type = htonl(PROTO_HELLO);
  hdr->len = sizeof(int);
  int real_len = hdr->len;
  hdr->len = htons(hdr->len);

  int *data = (int *)&hdr[1];
  *data = htonl(1);

  write(cfd, hdr, sizeof(proto_hdr_t) + real_len);
}

int main() {
  struct sockaddr_in server_info = {0};
  struct sockaddr_in client_info = {0};
  socklen_t client_size = 0;

  server_info.sin_family = AF_INET;
  // any address coming from local computer, e.g. localhost
  server_info.sin_addr.s_addr = INADDR_ANY;
  server_info.sin_port = htons(5555);

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("socket");
    return -1;
  }

  if (bind(fd, (struct sockaddr *)&server_info, sizeof(server_info)) == -1) {
    perror("bind");
    close(fd);
    return -1;
  }

  if (listen(fd, 0) == -1) {
    perror("listen");
    close(fd);
    return -1;
  }

  while (1) {
    int cfd = accept(fd, (struct sockaddr *)&client_info, &client_size);
    if (cfd == -1) {
      perror("accept");
      close(fd);
      return -1;
    }

    handle_client(cfd);
    close(cfd);
  }

  printf("Socket created with fd: %d\n", fd);
  return 0;
}
