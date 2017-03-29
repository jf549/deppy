#ifndef STRIDE_DETECTOR_H
#define STRIDE_DETECTOR_H

#include "MemoryRef.h"

#include <cstdint>

namespace analyser {
  enum class State { start, firstObs, strideLnd, weakStride, strongStride };

  class StrideDetector {
  public:
    StrideDetector();

    Stride getStride() const;
    void addAddress(uint64_t addr);

  private:
    State state;
    Stride stride;
  };
}

#endif // STRIDE_DETECTOR_H
