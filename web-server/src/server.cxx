#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <iostream>

int server_fd = -1;

void handle_sigint(int) {
  std::cout << "\nServer shutting down...\n";
  if (server_fd != -1) close(server_fd);
  exit(0);
}

int main() {
  signal(SIGINT, handle_sigint);

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("socket");
    return 1;
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(9000);        // port 9000
  addr.sin_addr.s_addr = INADDR_ANY;  // 0.0.0.0

  if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("bind");
    return 1;
  }

  if (listen(server_fd, 1) < 0) {
    perror("listen");
    return 1;
  }

  std::cout << "Server listening on port 9000...\n";

  sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
  if (client_fd < 0) {
    perror("accept");
    return 1;
  }

  char buffer[1024] = {};
  int bytes = read(client_fd, buffer, sizeof(buffer) - 1);
  if (bytes > 0) {
    std::cout << "Received from client: " << buffer << "\n";

    std::string reply = "Hello from server!";
    send(client_fd, reply.c_str(), reply.size(), 0);
  }

  close(client_fd);
  close(server_fd);
  return 0;
}
