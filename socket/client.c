#include "stdio.h"
#include <arpa/inet.h>
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
  read(cfd, buf, sizeof(proto_hdr_t) + sizeof(int));

  proto_hdr_t *hdr = (proto_hdr_t *)buf;
  hdr->type = ntohl(hdr->type);
  hdr->len = ntohs(hdr->len);

  int *data = (int *)&hdr[1];
  *data = ntohl(*data);

  if (hdr->type != PROTO_HELLO) {
    printf("Protocal mismatch, failing.\n");
    return;
  }

  if (*data != 1) {
    printf("Protocal version mismatch, failing.\n");
    return;
  }

  printf("Server connected, protocol v1\n");
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <ip of the host>\n", argv[0]);
    return 0;
  }

  struct sockaddr_in server_info = {0};

  server_info.sin_family = AF_INET;
  server_info.sin_addr.s_addr = inet_addr(argv[1]);
  server_info.sin_port = htons(5555);

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("socket");
    return -1;
  }

  if (connect(fd, (struct sockaddr *)&server_info, sizeof(server_info)) == -1) {
    perror("connect");
    close(fd);
    return -1;
  }

  handle_client(fd);
  close(fd);

  return 0;
}
