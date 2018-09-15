#ifndef RXVT_UTIL_H
#define RXVT_UTIL_H

#include <cstdlib>
#include <cstring>

#include "./ecb.h"

#include "emman.h"

// linear interpolation
template<typename T, typename U, typename P>
static inline T
lerp (T a, U b, P p)
{
  return (long(a) * long(100 - p) + long(b) * long(p) + 50) / 100;
}

// return a very temporary (and never deallocated) buffer. keep small.
void *rxvt_temp_buf (int len);

template<typename T>
static inline T *
rxvt_temp_buf (int len)
{
  return (T *)rxvt_temp_buf (len * sizeof (T));
}

// in range including end
#define IN_RANGE_INC(val,beg,end) \
  ((unsigned int)(val) - (unsigned int)(beg) <= (unsigned int)(end) - (unsigned int)(beg))

// in range excluding end
#define IN_RANGE_EXC(val,beg,end) \
  ((unsigned int)(val) - (unsigned int)(beg) <  (unsigned int)(end) - (unsigned int)(beg))

// for m >= -n, ensure remainder lies between 0..n-1
#define MOD(m,n) (((m) + (n)) % (n))

#endif
