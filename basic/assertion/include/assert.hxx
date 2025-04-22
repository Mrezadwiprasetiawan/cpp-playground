#pragma once
#include <cstdlib>

#define assert(b)\
  if(!(b)) std::exit(EXIT_FAILURE);
