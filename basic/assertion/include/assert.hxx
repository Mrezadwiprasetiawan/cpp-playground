#ifndef __ASSERT_HXX__
#define __ASSERT_HXX__
#include <cstdlib>

#define assert(b)\
  if(!(b)) std::exit(EXIT_FAILURE);

#endif
