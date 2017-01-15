#include "DynamicGcd.h"

#include <utility>
#include <algorithm>

using namespace analyser;

// See CLRS3
uint64_t egcd(uint64_t a, uint64_t b, uint64_t* x, uint64_t* y) {
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

// See CLRS3
uint64_t gcd(uint64_t a, uint64_t b) {
  while (b > 0) {
    uint64_t r = a % b; // If b > a then a % b = a, so a and b are swapped as required
    a = b;
    b = r;
  }

  return a;
}

uint64_t analyser::dynamicGcd(uint64_t low1, uint64_t low2, uint64_t high1, uint64_t high2, uint64_t stride1,
                    uint64_t stride2) {
  if (low2 < low1) {
    std::swap(low1, low2);
    std::swap(high1, high2);
    std::swap(stride1, stride2);
  }

  uint64_t low = std::max(low1, low2);
  uint64_t high = std::min(high1, high2);
  uint64_t length = 1 + high - low;
  uint64_t delta = (stride1 - ((low - low1) % stride1)) % stride1;

  if (gcd(stride1, stride2) % delta != 0) {
    return 0;
  }

  uint64_t x, y;
  uint64_t gcd = egcd(stride1, stride2, &x, &y);
  uint64_t lcm = (stride1 / gcd) * stride2; // gcd divides both, more efficient to divide only one.
  uint64_t offset = (stride2 * y * delta/gcd + lcm) % lcm;
  uint64_t result = (lcm + length - (offset + 1)) / lcm;

  // TODO: avg occur count total = access in stride / distinct access in stride

  return std::max((uint64_t) 0, result);
}

int main(void) {
  printf("%llu\n", dynamicGcd(20, 21, 32, 39, 2, 3));
}
