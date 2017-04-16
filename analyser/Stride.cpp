#include "Stride.h"
#include <lib/gcd.h>

#include <boost/math/common_factor_rt.hpp>

#include <algorithm>

namespace analyser {

  unsigned int Stride::numDependences(const Stride& other) const {
    if (other.base < base) {
      return other.numDependences(*this);
    }

    auto low = std::max(base, other.base);
    auto high = std::min(limit, other.limit);

    if (high < low) {
      return 0;
    }

    auto delta = (stride - ((low - base) % stride)) % stride;

    if (delta % boost::math::gcd(stride, other.stride) != 0) {
      return 0;
    }

    auto length = high - low + 1;

    auto res = lib::egcd(stride, other.stride);
    auto lcm = (stride / res.gcd) * other.stride; // gcd divides both; more efficient to divide only one
    auto offset = (other.stride * res.y * delta / res.gcd + lcm) % lcm;
    auto result = (length - (offset + 1) + lcm) / lcm;

    return result;
  }

  bool Stride::isDependent(uint64_t addr) const {
    return (base <= addr) && (addr <= limit) && ((addr - base) % stride == 0);
  }

}
