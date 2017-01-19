#include <stdint.h>

namespace lib {
  /// Find the GCD of a and b using the iterative version of Euclid's algorithm.
  uint64_t gcd(uint64_t a, uint64_t b);

  /// Perform the Extended Euclidian algorithm on a and b. Returns the GCD of a and b.
  /// x and y are return values.
  uint64_t egcd(uint64_t a, uint64_t b, uint64_t* x, uint64_t* y);
}
