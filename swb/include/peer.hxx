#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <vector>

#include "uint512_t.hxx"

/*
 */
struct NetPool {
  std::vector<uint512_t> Id;
  std::vector<int> MostNewOffset;
};