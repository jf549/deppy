#ifndef STRIDE_DETECTOR_H
#define STRIDE_DETECTOR_H

#include <cstdint>

namespace analyser {

  class StrideDetector {
  public:
    StrideDetector();

    // Add a memory reference to this stride detector. Returns true if the reference is part of the
    // current stride.
    bool addAddress(const uint64_t addr);

    // Returns true iff the StrideDetector is in an accepting state.
    bool isAccepting() const;

    // Get the stride distance of the currently detected stride.
    uint64_t getStride() const { return stride; }

    enum class State { start, firstObs, strideLnd, weakStride, strongStride };

  private:
    bool isInLearnedStride(const uint64_t addr) const;

    uint64_t base, stride, limit;
    State state;
  };

}

#endif // STRIDE_DETECTOR_H
