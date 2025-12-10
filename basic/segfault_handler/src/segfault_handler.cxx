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

#include <csignal>
#include <cstdlib>
#include <iostream>

#include <iostream>
#include <csignal>
#include <cstdlib>

void handler(int sig, siginfo_t *info, void *context) {
  std::cout << "signo: " << info->si_signo << "\n";
  std::cout << "errno: " << info->si_errno << "\n";
  std::cout << "code : " << info->si_code << "\n";

  std::cout << "pid  : " << info->si_pid << "\n";
  std::cout << "uid  : " << info->si_uid << "\n";
  std::cout << "status: " << info->si_status << "\n";
  std::cout << "utime: " << info->si_utime << "\n";
  std::cout << "stime: " << info->si_stime << "\n";

  std::cout << "value int : " << info->si_value.sival_int << "\n";
  std::cout << "value ptr : " << info->si_value.sival_ptr << "\n";

  std::cout << "addr : " << info->si_addr << "\n";
  std::cout << "band : " << info->si_band << "\n";
  std::cout << "fd   : " << info->si_fd << "\n";

  std::cout << "--- specific fields by si_code ---\n";

  if (info->si_code == SEGV_MAPERR || info->si_code == SEGV_ACCERR) {
    std::cout << "fault addr: " << info->si_addr << "\n";
  }

  if (info->si_code == SI_USER || info->si_code == SI_QUEUE) {
    std::cout << "sender pid: " << info->si_pid << "\n";
    std::cout << "sender uid: " << info->si_uid << "\n";
  }

  if (info->si_code == CLD_EXITED || info->si_code == CLD_KILLED ||
      info->si_code == CLD_DUMPED || info->si_code == CLD_STOPPED ||
      info->si_code == CLD_CONTINUED) {
    std::cout << "child pid: " << info->si_pid << "\n";
    std::cout << "child uid: " << info->si_uid << "\n";
    std::cout << "status   : " << info->si_status << "\n";
    std::cout << "utime    : " << info->si_utime << "\n";
    std::cout << "stime    : " << info->si_stime << "\n";
  }

  if (info->si_code == ILL_ILLOPC || info->si_code == ILL_ILLOPN ||
      info->si_code == ILL_ILLADR || info->si_code == ILL_ILLTRP ||
      info->si_code == ILL_PRVOPC || info->si_code == ILL_PRVREG ||
      info->si_code == ILL_COPROC || info->si_code == ILL_BADSTK) {
    std::cout << "ill addr: " << info->si_addr << "\n";
  }

  if (info->si_code == FPE_INTDIV || info->si_code == FPE_INTOVF ||
      info->si_code == FPE_FLTDIV || info->si_code == FPE_FLTOVF ||
      info->si_code == FPE_FLTUND || info->si_code == FPE_FLTRES ||
      info->si_code == FPE_FLTINV || info->si_code == FPE_FLTSUB) {
    std::cout << "fpe addr: " << info->si_addr << "\n";
  }

  if (info->si_code == BUS_ADRALN || info->si_code == BUS_ADRERR ||
      info->si_code == BUS_OBJERR) {
    std::cout << "bus addr: " << info->si_addr << "\n";
  }

  if (info->si_code == POLL_IN || info->si_code == POLL_OUT ||
      info->si_code == POLL_MSG || info->si_code == POLL_ERR ||
      info->si_code == POLL_PRI || info->si_code == POLL_HUP) {
    std::cout << "band: " << info->si_band << "\n";
    std::cout << "fd  : " << info->si_fd << "\n";
  }

  std::exit(1);
}

int main() {
  struct sigaction sa;
  sa.sa_flags     = SA_SIGINFO;
  sa.sa_sigaction = handler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGSEGV, &sa, nullptr);

  int *p = (int *)0xDEADBEEF;  // alamat ilegal
  *p     = 42;

  return 0;
}
