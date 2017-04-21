#ifndef POINT_LOOP_H
#define POINT_LOOP_H

#include "Loop.h"
#include "Point.h"

#include <cstdint>
#include <map>
#include <set>
#include <vector>

namespace analyser {

  class PointLoop : public Loop {
  public:
    // Construct a top level loop.
    PointLoop();

    // Construct a nested loop by providing a pointer to its parent loop.
    explicit PointLoop(PointLoop* parent);

  protected:
    using PointTableT = std::map<uint64_t /* addr */, std::vector<Point>>;

    // Propagate dependence history from a child of this loop upon termination of the child.
    void propagate(const PointLoop& childLoop);

    // Find dependences and merge tables in O(N*log(M)) time, where N is the size of the pending
    // table and M the size of the history table.
    void findPointPointDependences();

    void mergePointTables();

    PointTableT pendingPointTable, historyPointTable;
    std::set<uint64_t> killedAddrs;
    unsigned int iter;

  private:
    virtual void doDependenceCheck() override;
    virtual void doIteration() override;
    virtual void doPropagation() override;
    virtual void addMemoryRef(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses) override;

    PointLoop* const parent;
  };

}

#endif // POINT_LOOP_H
