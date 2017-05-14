#include "Loop.h"

namespace analyser {

  Loop::Loop(bool detailedResults)
    : results(detailedResults
              ? std::unique_ptr<DependenceResults>{ std::make_unique<DetailedDependenceResults>() }
              : std::unique_ptr<DependenceResults>{ std::make_unique<BasicDependenceResults>() } )
    {}

  Loop::~Loop() = default;

  void Loop::iterate() {
    doDependenceCheck();
    doIteration();
  }

  std::unique_ptr<DependenceResults> Loop::terminate() {
    doDependenceCheck();
    doPropagation();
    return std::move(results);
  }

  void Loop::memoryRef(PcT pc, AddrT addr, bool isWrite) {
    addMemoryRef(pc, addr, isWrite);
  }

}
