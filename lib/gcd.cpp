#include "gcd.h"

using namespace lib;

// See CLRS3
uint64_t lib::gcd(uint64_t a, uint64_t b) {
  while (b > 0) {
    uint64_t r = a % b; // If b > a then a % b = a, so a and b are swapped as required
    a = b;
    b = r;
  }

  return a;
}

// See CLRS3
uint64_t lib::egcd(uint64_t a, uint64_t b, uint64_t* x, uint64_t* y) {
  uint64_t x0 = 1, x1 = 0, y0 = 0, y1 = 1;

  while (b > 0) {
    uint64_t q = a / b;
    uint64_t r = a % b;
    a = b;
    b = r;
    r = x0 - q * x1;
    x0 = x1;
    x1 = r;
    r = y0 - q * y1;
    y0 = y1;
    y1 = r;

  }

  *x = x0;
  *y = y0;
  return a;
}
