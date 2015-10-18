#ifndef __UTILS_H__
#define __UTILS_H__

#include <algorithm>

template<class T>
inline T max_element(const T* __restrict__ p, size_t s)
{
  T r = p[0];
  for (size_t n = 1; n < s; n++) {
    r = std::max(r,p[n]);
  }
  return r;
};

template<class T0, class T1>
inline T0 reduce(const T1* __restrict__ p, size_t s, const T0 init)
{
  T0 r = init;
  for (size_t n = 0; n < s; n++) {
    r += static_cast<T0>(p[n]);
  }
  return r;
};

template<class T0, class T1, class T2>
inline T0 inner_product(const T1* __restrict__ p1, const T2* __restrict__ p2, size_t s, T0 init)
{
  T0 r = init;
  for (size_t n = 0; n < s; n++) {
    r += static_cast<T0>(p1[n] * p2[n]);
  }
  return r;
};

template<class T0>
inline void memcpy(const T0* __restrict__ p1, T0* __restrict__ p2, const size_t s)
{
  for (size_t n = 0; n < s; n++) {
    p2[n] = p1[n];
  }
}

#endif // __UTILS_H__

