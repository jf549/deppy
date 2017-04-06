#ifndef LOOP_H
#define LOOP_H

#include "Point.h"

#include <map>
#include <set>
#include <vector>

namespace analyser {

  class Loop {
  public:
    // Construct a top level loop.
    Loop();

    // Construct a nested loop by providing a pointer to its parent loop.
    Loop(Loop* parent);

    // Call each time a LoopIter event is seen for this loop.
    virtual void iterate();

    // Call when the LoopEnd event is seen for this loop.
    // TODO could use destructor instead?
    virtual void terminate();

    // Call each time a memory access event is seen inside this loop.
    virtual void memoryRef(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses = 1);

  protected:
    // TODO would an unordered_map be better?
    using PointTableT = std::map<uint64_t /* addr */, std::vector<Point>>;

    // Propagate dependence history from a child of this loop upon termination of the child.
    void propagate(const PointTableT& childHistoryPointTable);

    // Report a loop-dependent dependence.
    void reportDependence(uint64_t sourcePc, uint64_t sinkPc, bool sourceIsWrite, bool sinkIsWrite,
                          unsigned int sourceIter, unsigned int sinkIter) const;

    // Report a loop-independent dependence.
    void reportIndependentDependence(uint64_t srcPc, uint64_t sinkPc, bool srcIsWrite,
                                     bool sinkIsWrite) const;

    // Find dependences and merge tables in O(N*log(M)) time where N is the size of the pending
    // table and M the size of the history table.
    void dependenceCheckAndMerge();

    unsigned int iter;
    Loop* parent;
    PointTableT pendingPointTable, historyPointTable;
    std::set<uint64_t> killedAddrs;
  };

}

#endif // LOOP_H
