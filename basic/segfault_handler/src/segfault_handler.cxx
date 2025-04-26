#include <csignal>
#include <iostream>
#include <cstdlib>

void handler(int sig, siginfo_t *info, void *context) {
  std::cout << "Segfault at address: " << info->si_addr << std::endl;
  std::exit(1);
}

int main() {
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = handler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGSEGV, &sa, nullptr);

  int *p = (int*)0xDEADBEEF; // alamat ilegal
  *p = 42;

  return 0;
}
