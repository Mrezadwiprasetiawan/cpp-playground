#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

int main() {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    return 1;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(9000);
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

  if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("connect");
    return 1;
  }

  std::string msg = "Hello from client!";
  send(sock, msg.c_str(), msg.size(), 0);

  char buffer[1024] = {};
  int bytes = read(sock, buffer, sizeof(buffer) - 1);
  if (bytes > 0) { std::cout << "Received from server: " << buffer << "\n"; }

  close(sock);
  return 0;
}
