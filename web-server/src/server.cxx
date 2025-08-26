/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan


  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/


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
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(9000);  // port 9000
  addr.sin_addr.s_addr = INADDR_ANY;   // 0.0.0.0

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
  socklen_t   client_len = sizeof(client_addr);
  int         client_fd  = accept(server_fd, (sockaddr*)&client_addr, &client_len);
  if (client_fd < 0) {
    perror("accept");
    return 1;
  }

  char buffer[1024] = {};
  int  bytes        = read(client_fd, buffer, sizeof(buffer) - 1);
  if (bytes > 0) {
    std::cout << "Received from client: " << buffer << "\n";

    std::string reply = "Hello from server!";
    send(client_fd, reply.c_str(), reply.size(), 0);
  }

  close(client_fd);
  close(server_fd);
  return 0;
}
