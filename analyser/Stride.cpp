#include "Stride.h"

#include <gmp.h>

#include <algorithm>

namespace analyser {

  AddrT Stride::numDependences(const Stride& other) const {
    if (other.base < base) {
      return other.numDependences(*this);
    }

    mpz_t dist0, dist1, result;

    auto low = other.base;
    auto high = std::min(limit, other.limit);

    if (high < low) {
      return 0;
    }

    auto delta = (stride - ((low - base) % stride)) % stride;

    mpz_init_set_ui(dist0, stride);

    unsigned long gcd = mpz_gcd_ui(nullptr, dist0, other.stride);

    if (delta % gcd != 0) {
      mpz_clear(dist0);
      return 0;
    }

    mpz_init(result);
    mpz_init_set_ui(dist1, other.stride);

    mpz_gcdext(dist0, result, nullptr, dist1, dist0);

    // dist1 now contains the lcm of dist0 and dist1
    mpz_mul_ui(dist1, dist1, stride/gcd); // gcd divides both; more efficient to divide only one

    mpz_mul_ui(result, result, delta/gcd);
    mpz_mul_ui(result, result, other.stride);
    mpz_add(result, result, dist1);
    mpz_mod(result, result, dist1);
    mpz_ui_sub(result, high - low, result);
    mpz_add(result, result, dist1);
    mpz_tdiv_q(result, result, dist1);

    if (mpz_cmp_si(result, 0) <= 0) {
      mpz_clears(dist0, dist1, result, nullptr);
      return 0;
    } else {
      unsigned long res = mpz_get_ui(result);
      mpz_clears(dist0, dist1, result, nullptr);
      return res;
    }
  }

  bool Stride::isDependent(AddrT addr) const {
    return (base <= addr) && (addr <= limit) && ((addr - base) % stride == 0);
  }

  bool Stride::merge(const Stride& other) {
    if ((base < other.base ? other.base - base : base - other.base) % stride == 0) {
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
