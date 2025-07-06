#include <netinet/in.h>  // sockaddr_in
#include <sys/socket.h>  // socket(), bind(), listen(), accept()
#include <unistd.h>      // close()

#include <cstring>  // memset
#include <iostream>

namespace WS {
class TCP {
  std::string hostname;
  uint8_t     port;
};
}  // namespace WS