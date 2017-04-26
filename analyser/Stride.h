#ifndef STRIDE_H
#define STRIDE_H

#include "Point.h"

#include <cstdint>

namespace analyser {

  struct Stride {
    // Returns the number of dependences between this stride and the provided stride using the
    // Dynamic-GCD algorithm.
    uint64_t numDependences(const Stride& other) const;

    // Returns true iff there is a dependence between this stride and the supplied point.
    bool isDependent(uint64_t addr) const;

    uint64_t base, stride, limit;
    unsigned int numAccesses, iterLastAccessed;
    bool isWrite;
  };

}

#endif // STRIDE_H
