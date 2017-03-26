#ifndef STRIDE_DETECTOR_H
#define STRIDE_DETECTOR_H

#include <stdint.h>

namespace analyser {
  enum class State : int {start, firstObs, strideLnd, weakStride, strongStride};

  struct Stride {
    uint64_t base, diff, limit;
  };

  class StrideDetector {
  public:
    StrideDetector(uint64_t addr);

    Stride getStride() {return stride;}
    void addAddress(uint64_t addr);

  private:
    State s;
    Stride stride;
  };
}

#endif // STRIDE_DETECTOR_H
