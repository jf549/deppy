#ifndef STRIDE_LOOP_H
#define STRIDE_LOOP_H

#include "PointLoop.h"
#include "Stride.h"
#include "StrideDetector.h"

#include <cstdint>
#include <map>
#include <vector>

namespace analyser {

  class StrideLoop : public PointLoop {
  public:
    // Construct a top level loop.
    StrideLoop();

    // Construct a nested loop by providing a pointer to its parent loop.
    StrideLoop(StrideLoop* parent);

  protected:
    using StrideTableT = std::map<uint64_t /* pc */, std::vector<Stride>>;

    // Propagate dependence history from a child of this loop upon termination of the child.
    void propagate(StrideLoop& childLoop);

    void findStrideStrideDependences();

    void findStridePointDependences();

    void findPointStrideDependences();

    void mergeStride(std::vector<Stride>& strides, const Stride& toMerge) const;

    // Merge the pendingStrideTable into the historyStrideTable.
    void mergeStrideTables();

    StrideTableT pendingStrideTable, historyStrideTable;
    std::map<uint64_t /* pc */, StrideDetector> detectors;

  private:
    virtual void doDependenceCheck() override;
    virtual void doPropagation() override;
    virtual void addMemoryRef(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses) override;

    StrideLoop* const parent;
  };

}

#endif // STRIDE_LOOP_H
