#ifndef STRIDE_LOOP_H
#define STRIDE_LOOP_H

#include "PointLoop.h"
#include "Stride.h"
#include "StrideDetector.h"

#include <ext/intervaltree/IntervalTree.h>

#include <list>
#include <map>

namespace analyser {

  class StrideLoop : public PointLoop {
  public:
    // Construct a top level loop.
    StrideLoop(bool detailedResults = false);

    // Construct a nested loop by providing a pointer to its parent loop.
    StrideLoop(StrideLoop* parent, bool detailedResults = false);

  protected:
    using StrideListT = std::list<Stride>;
    using StrideTableT = std::map<PcT, StrideListT>;

    using IntervalT = Interval<Stride*, AddrT>;
    using IntervalTreeT = IntervalTree<Stride*, AddrT>;

    // Propagate dependence history from a child of this loop upon termination of the child.
    void propagate(const StrideLoop& childLoop);

    void findStrideStrideDependences() const;

    void findStridePointDependences() const;

    void findPointStrideDependences() const;

    void mergeStride(StrideListT& strides, const Stride& toMerge);

    // Merge the pendingStrideTable into the historyStrideTable.
    void mergeStrideTables();

    StrideTableT pendingStrideTable, historyStrideTable;
    std::map<PcT, StrideDetector> detectors;
    IntervalTreeT intervalTree;

  private:
    virtual void doDependenceCheck() override;
    virtual void doPropagation() const override;
    virtual void addMemoryRef(PcT pc, AddrT addr, bool isWrite) override;

    StrideLoop* const parent;
  };

}

#endif // STRIDE_LOOP_H
