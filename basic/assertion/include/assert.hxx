#pragma once

#include <cstdio>
#include <cstdlib>

#define assert(b)               \
  if (!(b)) {                   \
    printf("Error : %s\n", #b); \
    exit(EXIT_FAILURE);         \
  }
