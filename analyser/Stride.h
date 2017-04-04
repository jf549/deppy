#ifndef STRIDE_H
#define STRIDE_H

#include <cstdint>

namespace analyser {

  struct Stride {
    uint64_t base, stride, limit;
    bool isWrite;
    unsigned int numAccesses;
    unsigned int iterLastAccessed;

    // Returns the number of dependences between this stride and the provided stride using the
    // Dynamic GCD algorithm.
    unsigned int numDependences(const Stride& other) const;
  };

}

#endif // STRIDE_H
