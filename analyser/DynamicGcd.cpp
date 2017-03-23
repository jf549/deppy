#include "DynamicGcd.h"
#include "../lib/gcd.h"

#include <boost/math/common_factor_rt.hpp>

#include <utility>
#include <algorithm>

using namespace analyser;

uint64_t analyser::dynamicGcd(uint64_t low1, uint64_t low2, uint64_t high1, uint64_t high2,
                              uint64_t dist1, uint64_t dist2) {
  if (low2 < low1) {
    std::swap(low1, low2);
    std::swap(high1, high2);
    std::swap(dist1, dist2);
  }

  uint64_t low = std::max(low1, low2);
  uint64_t high = std::min(high1, high2);
  uint64_t length = high - low + 1;
  uint64_t delta = (dist1 - ((low - low1) % dist1)) % dist1;

  if (delta % boost::math::gcd(dist1, dist2) != 0) {
    return 0;
  }

  auto res = lib::egcd(-dist1, dist2);
  uint64_t lcm = (dist1 / res.gcd) * dist2; // gcd divides both; more efficient to divide only one
  uint64_t offset = (dist2 * res.y * delta / res.gcd + lcm) % lcm;
  uint64_t result = (length - (offset + 1) + lcm) / lcm;

  return std::max<uint64_t>(0, result);
}

int main() {
  printf("%llu\n", dynamicGcd(20, 21, 32, 39, 2, 3));
}
