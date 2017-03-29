#ifndef LOOP_H
#define LOOP_H

#include "MemoryRef.h"

#include <map>
#include <set>

namespace analyser {

  class Loop {
    using PointTableT = std::map<uint64_t /* addr */, Point>;

  public:
    Loop();
    Loop(Loop* p);

    void iterate();

    void terminate();

    void memoryRef(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses = 1);

  private:
    void propagate(const PointTableT& childHistoryPointTable);

    unsigned int iter;
    Loop* parent;
    // TODO would an unordered_map be better?
    PointTableT pendingPointTable, historyPointTable;
    std::set<uint64_t /* addr */> killedAddrs;
  };

}

#endif // LOOP_H
