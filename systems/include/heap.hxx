/*
  Cross-platform available memory checker
  Returns available physical memory (bytes)
*/

#include <cstddef>

#if defined(_WIN32)
#include <windows.h>

inline size_t get_available_heap() {
  MEMORYSTATUSEX statex{};
  statex.dwLength = sizeof(statex);
  if (GlobalMemoryStatusEx(&statex)) {
    return static_cast<size_t>(statex.ullAvailPhys);  // RAM fisik tersedia
  }
  return 0;
}

#elif defined(__linux__)
#include <fstream>
#include <string>

inline size_t get_available_heap() {
  std::ifstream meminfo("/proc/meminfo");
  std::string   line;
  while (std::getline(meminfo, line)) {
    if (line.rfind("MemAvailable:", 0) == 0) {
      size_t kb = 0;
      try {
        kb = std::stoul(line.substr(13));
      } catch (...) { kb = 0; }
      return kb * 1024;  // kB → byte
    }
  }
  return 0;
}

#elif defined(__APPLE__)
#include <mach/mach.h>

inline size_t get_available_heap() {
  mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
  vm_statistics64_data_t vmstat;
  if (host_statistics64(mach_host_self(), HOST_VM_INFO64, reinterpret_cast<host_info64_t>(&vmstat), &count) == KERN_SUCCESS) {
    return static_cast<size_t>(vmstat.free_count + vmstat.inactive_count) * sysconf(_SC_PAGESIZE);
  }
  return 0;
}

#else
inline size_t get_available_heap() {
  return 0;  // not supported
}
#endif
