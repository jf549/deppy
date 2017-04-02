#include "StrideLoop.h"

namespace analyser {

  void StrideLoop::iterate() {
    Loop::iterate();
  }

  void StrideLoop::terminate() {
    Loop::terminate();
  }

  void StrideLoop::memoryRef(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses) {
    Loop::memoryRef(pc, addr, isWrite, numAccesses);
  }

  void StrideLoop::propagate(const PointTableT& childPointTable,
                             const StrideTableT& childStrideTable) {
    Loop::propagate(childPointTable);
  }

}
