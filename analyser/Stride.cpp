#include "Stride.h"

#include <gmpxx.h>

#include <algorithm>

namespace analyser {

  uint64_t Stride::numDependences(const Stride& other) const {
    if (other.base < base) {
      return other.numDependences(*this);
    }

    mpz_class low0 = static_cast<unsigned long>(base);
    mpz_class dist0 = static_cast<unsigned long>(stride);
    mpz_class dist1 = static_cast<unsigned long>(other.stride);
    mpz_class low = static_cast<unsigned long>(other.base);
    mpz_class high = static_cast<unsigned long>(std::min(limit, other.limit));

    if (high < low) {
      return 0;
    }

    mpz_class delta = (dist0 - ((low - low0) % dist0)) % dist0;

    if (delta % gcd(dist0, dist1) != 0) {
      return 0;
    }

    mpz_class gcd, x, y;
    mpz_gcdext(gcd.get_mpz_t(), x.get_mpz_t(), y.get_mpz_t(), dist0.get_mpz_t(), dist1.get_mpz_t());

    mpz_class lcm = (dist0 / gcd) * dist1; // gcd divides both; more efficient to divide only one
    mpz_class offset = (dist1 * y * delta / gcd + lcm) % lcm;
    mpz_class result = (high - low + 1 - (offset + 1) + lcm) / lcm;

    if (result < 0) {
      return 0;
    } else {
      return result.get_ui();
    }
  }

  bool Stride::isDependent(uint64_t addr) const {
    return (base <= addr) && (addr <= limit) && ((addr - base) % stride == 0);
  }

  bool Stride::merge(const Stride& other) {
    if (isWrite == other.isWrite && stride == other.stride
        && (base < other.base ? other.base - base : base - other.base) % stride == 0) {

      if (other.base < base) {
        base = other.base;
      }

      if (other.limit > limit) {
        limit = other.limit;
      }

      if (other.iterLastAccessed > iterLastAccessed) {
        iterLastAccessed = other.iterLastAccessed;
      }

      numAccesses += other.numAccesses;

      return true;
    }

    return false;
  }

}
