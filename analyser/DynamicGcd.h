#include <stdint.h>

namespace analyser {
  /// Given two strides, returns the number of dependences between them using a GCD test.
  uint64_t dynamicGcd(uint64_t low1, uint64_t low2, uint64_t high1, uint64_t high2, uint64_t dist1,
                      uint64_t dist2);
}
