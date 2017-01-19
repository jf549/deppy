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

uint64_t analyser::dynamicGcd(uint64_t low1, uint64_t low2, uint64_t high1, uint64_t high2, uint64_t dist1,
                    uint64_t dist2) {
  if (low2 < low1) {
    std::swap(low1, low2);
    std::swap(high1, high2);
    std::swap(dist1, dist2);
  }

  uint64_t low = std::max(low1, low2);
  uint64_t high = std::min(high1, high2);
  uint64_t length = high - low + 1;
  uint64_t delta = (dist1 - ((low - low1) % dist1)) % dist1;

  if (gcd(dist1, dist2) % delta != 0) {
    return 0;
  }

  uint64_t x, y;
  uint64_t gcd = egcd(dist1, dist2, &x, &y);
  uint64_t lcm = (dist1 / gcd) * dist2; // gcd divides both, more efficient to divide only one.
  uint64_t offset = (dist2 * y * delta/gcd + lcm) % lcm;
  uint64_t result = (lcm + length - (offset + 1)) / lcm;

  return std::max((uint64_t) 0, result);
}

int main(void) {
  printf("%llu\n", dynamicGcd(20, 21, 32, 39, 2, 3));
}
