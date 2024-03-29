#ifndef POINT_LOOP_H
#define POINT_LOOP_H

#include "Loop.h"
#include "Point.h"

#include <map>
#include <set>
#include <vector>

namespace analyser {

  class PointLoop : public Loop {
  public:
    // Construct a top level loop.
    PointLoop(bool detailedResults = false);

    // Construct a nested loop by providing a pointer to its parent loop.
    explicit PointLoop(PointLoop* parent, bool detailedResults = false);

  protected:
    using PointTableT = std::map<AddrT, std::vector<Point>>;

    // Propagate dependence history from a child of this loop upon termination of the child.
    void propagate(const PointLoop& childLoop);

    // Find dependences and merge tables in O(N*log(M)) time, where N is the size of the pending
    // table and M the size of the history table.
    void findPointPointDependences() const;

    void mergePointTables();

    PointTableT pendingPointTable, historyPointTable;
    std::set<AddrT> killedAddrs;
    unsigned int iter;

  private:
    virtual void doDependenceCheck() override;
    virtual void doIteration() override;
    virtual void doPropagation() const override;
    virtual void addMemoryRef(PcT pc, AddrT addr, bool isWrite) override;

    PointLoop* const parent;
  };

}

#endif // POINT_LOOP_H
