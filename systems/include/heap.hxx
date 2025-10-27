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
