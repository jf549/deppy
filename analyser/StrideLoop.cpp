#include "StrideLoop.h"

namespace analyser {

  StrideLoop::StrideLoop() : Loop(), parent(nullptr) {}
  StrideLoop::StrideLoop(StrideLoop* p) : Loop(), parent(p) {}

  void StrideLoop::iterate() {
    Loop::iterate(); // Point-point dependences
    // TODO Point-Stride and Stride-Stride
  }

  void StrideLoop::terminate() {
    iterate();
    if (parent) {
      parent->propagate(historyPointTable, historyStrideTable);
    }
  }

  void StrideLoop::memoryRef(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses) {
    Loop::memoryRef(pc, addr, isWrite, numAccesses);
    //detectors[pc].addAddress(pc, addr, isWrite, numAccesses);
  }

  void StrideLoop::propagate(const PointTableT& childPointTable,
                             const StrideTableT& childStrideTable) {
    Loop::propagate(childPointTable);
  }

}
