#ifndef STRIDE_LOOP_H
#define STRIDE_LOOP_H

#include "Loop.h"
#include "MemoryRef.h"
#include "StrideDetector.h"

#include <map>
#include <vector>

namespace analyser {

  class StrideLoop : public Loop {
  public:
    using Loop::Loop;
    
    // Call each time a LoopIter event is seen for this loop.
    virtual void iterate() override;

    // Call when the LoopEnd event is seen for this loop.
    // TODO could use destructor instead?
    virtual void terminate() override;

    // Call each time a memory access event is seen inside this loop.
    virtual void memoryRef(uint64_t pc, uint64_t addr, bool isWrite,
                           unsigned int numAccesses = 1) override;

  protected:
    // TODO would an unordered_map be better?
    using StrideTableT = std::map<uint64_t /* pc */, std::vector<Stride>>;

    // Propagate dependence history from a child of this loop upon termination of the child.
    void propagate(const PointTableT& childPointTable, const StrideTableT& childStrideTable);

    StrideTableT pendingStrideTable, historyStrideTable;
    std::map<uint64_t /* pc */, StrideDetector> detectors;
  };

}

#endif // STRIDE_LOOP_H
