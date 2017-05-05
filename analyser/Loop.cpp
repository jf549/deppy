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

  void Loop::memoryRef(PcT pc, AddrT addr, bool isWrite) {
    addMemoryRef(pc, addr, isWrite);
  }

}
