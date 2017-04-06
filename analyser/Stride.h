#ifndef STRIDE_H
#define STRIDE_H

#include "Point.h"

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

    // Checks if there is a dependence between this stride and the supplied point.
    bool isDependent(uint64_t addr) const;
  };

}

#endif // STRIDE_H
