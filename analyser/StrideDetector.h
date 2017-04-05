#ifndef STRIDE_DETECTOR_H
#define STRIDE_DETECTOR_H

#include <cstdint>

namespace analyser {

  enum class State { start, firstObs, strideLnd, weakStride, strongStride };

  class StrideDetector {
  public:
    StrideDetector();

    // Add a memory reference to this stride detector. Returns true if the reference is part of the
    // current stride.
    bool addAddress(uint64_t addr);

  private:
    State state;
    uint64_t base, stride, limit;
  };

}

#endif // STRIDE_DETECTOR_H
