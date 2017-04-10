#include "Loop.h"

namespace analyser {

  void Loop::iterate() {
    doDependenceCheck();
    doIteration();
  }

  void Loop::terminate() {
    doDependenceCheck();
    doPropagation();
  }

  void Loop::memoryRef(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses) {
    addMemoryRef(pc, addr, isWrite, numAccesses);
  }

}
