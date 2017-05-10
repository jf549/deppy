#ifndef STRIDE_H
#define STRIDE_H

#include "types.h"

namespace analyser {

  struct Stride {
    // Returns the number of dependences between this stride and the provided stride using the
    // Dynamic-GCD algorithm.
    AddrT numDependences(const Stride& other) const;

    // Returns true iff there is a dependence between this stride and the supplied point.
    bool isDependent(AddrT addr) const;

    // Attempt to merge an overlapping stride with this one. Returns true if successful.
    bool merge(const Stride& other);

    AddrT base, stride, limit;
    unsigned int iterLastAccessed;
    bool isWrite;
  };

}

#endif // STRIDE_H
