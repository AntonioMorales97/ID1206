#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <assert.h>

#define NAME "pingpong"
#define TURNS 10

int main(void) {
  int sock;

  assert((sock = socket(AF_UNIX, SOCK_STREAM, 0)) != -1);

  struct sockaddr_un name = {AF_UNIX, NAME};

  assert(bind(sock, (struct sockaddr*)&name, sizeof(name)) != -1);

  assert(listen(sock, 5) != -1);

  int cfd;

  assert((cfd = accept(sock, NULL, NULL)) != -1);

  for(int i = 0; i < TURNS; i++) {
    char buffer[5];
    assert(send(cfd, "ping", 4, 0) != -1);
    assert(recv(cfd, buffer, 4, 0) != -1);
    buffer[4] = 0;
    printf("ping received %s\n", buffer);
  }

  unlink(NAME);
  return 0;
}
