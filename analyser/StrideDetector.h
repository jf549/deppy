#ifndef STRIDE_DETECTOR_H
#define STRIDE_DETECTOR_H

#include "Point.h"
#include "Stride.h"

#include <cstdint>
#include <map>

namespace analyser {
  enum class State { start, firstObs, strideLnd, weakStride, strongStride };

  class StrideDetector {
  public:
    StrideDetector();

    void addAddress(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses);

    using PointTableT = std::map<uint64_t /* addr */, std::vector<Point>>;
    using StrideTableT = std::map<uint64_t /* pc */, std::vector<Stride>>;
    void fillTables(PointTableT&, StrideTableT&);

  private:
    State state;
    Stride stride;
  };
}

#endif // STRIDE_DETECTOR_H
