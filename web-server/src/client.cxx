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
  server_addr.sin_port   = htons(9000);
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

  if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("connect");
    return 1;
  }

  std::string msg = "Hello from client!";
  send(sock, msg.c_str(), msg.size(), 0);

  char buffer[1024] = {};
  int  bytes        = read(sock, buffer, sizeof(buffer) - 1);
  if (bytes > 0) { std::cout << "Received from server: " << buffer << "\n"; }

  close(sock);
  return 0;
}
