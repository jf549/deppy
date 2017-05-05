#ifndef STRIDE_DETECTOR_H
#define STRIDE_DETECTOR_H

#include "types.h"

namespace analyser {

  class StrideDetector {
  public:
    StrideDetector();

    // Add a memory access to the stride detector. Returns true iff the address can be represented
    // with the learned stride.
    bool addAddress(const AddrT addr);

    // Returns true iff the StrideDetector is in an accepting state.
    bool isAccepting() const;

    // Get the stride distance of the learned stride.
    AddrT getStride() const;

    // Finite state machine state.
    enum class State { start, firstObs, strideLnd, weakStride, strongStride };

  private:
    // Return true iff the supplied address can be represented with the learned stride.
    bool isInLearnedStride(const AddrT addr) const;

    AddrT base, stride, limit;
    State state;
  };

}

#endif // STRIDE_DETECTOR_H
