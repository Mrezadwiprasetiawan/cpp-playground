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
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(8080);  // port 8080
  addr.sin_addr.s_addr = INADDR_ANY;   // 0.0.0.0

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
  socklen_t   client_len = sizeof(client_addr);
  int         client_fd  = accept(server_fd, (sockaddr*)&client_addr, &client_len);
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
