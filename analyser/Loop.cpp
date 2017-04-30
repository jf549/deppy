#include "Loop.h"

namespace analyser {

  Loop::Loop() = default;
  Loop::~Loop() = default;

  void Loop::iterate() {
    doDependenceCheck();
    doIteration();
  }

  void Loop::terminate() {
    doDependenceCheck();
    doPropagation();
  }

  void Loop::memoryRef(uint64_t pc, uint64_t addr, bool isWrite) {
    addMemoryRef(pc, addr, isWrite);
  }

}
