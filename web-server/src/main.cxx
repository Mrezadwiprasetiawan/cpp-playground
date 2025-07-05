#include <netinet/in.h>  // sockaddr_in
#include <sys/socket.h>  // socket(), bind(), listen(), accept()
#include <unistd.h>      // close()

#include <cstring>  // memset
#include <iostream>

int main() {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("socket failed");
    return 1;
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080);        // port 8080
  addr.sin_addr.s_addr = INADDR_ANY;  // 0.0.0.0

  if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("bind failed");
    return 1;
  }

  if (listen(server_fd, 1) < 0) {
    perror("listen failed");
    return 1;
  }

  std::cout << "Server ready on port 8080\n";

  sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
  if (client_fd < 0) {
    perror("accept failed");
    return 1;
  }

  const char* msg = "Hello from server!\n";
  send(client_fd, msg, strlen(msg), 0);

  close(client_fd);
  close(server_fd);
  return 0;
}
