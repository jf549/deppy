#include "Loop.h"

#include <iostream>

namespace analyser {

  Loop::Loop() : iter(0), parent(nullptr) {}
  Loop::Loop(Loop* p) : iter(0), parent(p) {}

  // Do the dependence checking and report any found dependences and merge the pending and history
  // point tables.
  // Finally, the pending table, including killed bits, is flushed.
  void Loop::iterate() {
    // if (iter == 0) {
    //   historyPointTable = std::move(pendingPointTable);
    //   // TODO can we move out of pendingPointTable and then reconstruct it afresh?
    // }
    for (auto& pair : pendingPointTable) {
      const auto addr = pair.first;
      auto& point = pair.second;

      if (historyPointTable.count(addr)) { // Dependence
        auto& historyPoint = pendingPointTable.at(addr);

        std::cout << "Loop-carried dependence: pc " << point.pc
                  << ", addr " << addr
                  << (point.isWrite ? ", write" : ", read") << '\n'; //TODO RAW, WAR, WAW

        if (historyPoint.pc == point.pc) {
          historyPoint.numAccesses += point.numAccesses;
          historyPoint.iterLastAccessed = iter;
        } else {
          historyPoint.next = std::make_unique<Point>(std::move(point));
        }

        // TODO but both point and historyPoint have nexts we need to check :(

      } else { // No dependence
        historyPointTable.emplace(std::move(pair)); // Also moves any next pointers
      }
    }

    pendingPointTable.clear();
    killedAddrs.clear();

    ++iter;
  }

  // To handle loop nests, we propagate the history table of L to the parent of L, if it exists.
  void Loop::terminate() {
    if (parent) {
      parent->propagate(historyPointTable);
    }
  }

  // On a memory reference, R, check the killed bit of R.
  // If killed, report a loop-independent dependence.
  // Otherwise, store R in PendingPointTable. If R is a write, set its killed bit.
  void Loop::memoryRef(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses) {
    if (killedAddrs.count(addr)) {
      std::cout << "Loop-independent dependence: pc " << pc
                << ", addr " << addr
                << (isWrite ? ", write" : ", read") << '\n';

    } else {
      if (pendingPointTable.count(addr)) {
        auto& point = pendingPointTable.at(addr);

        if (point.pc == pc) {
          point.numAccesses += numAccesses;
        } else {
          point.next = std::make_unique<Point>(Point{ pc, isWrite, numAccesses, iter, nullptr });
        }

      } else {
        pendingPointTable.emplace(addr, Point{ pc, isWrite, numAccesses, iter, nullptr });
      }

      if (isWrite) {
        killedAddrs.insert(addr);
      }
    }
  }

  // Propagation is done by merging the history table of L with the pending table of the parent of L.
  // To handle loop-independent dependences, if a memory address in the history tables of L
  // is killed by the parent of L, this killed history is not propagated.
  // We essential treat the inner loop as if it is fully unrolled inside its parent.
  void Loop::propagate(const PointTableT& childHistoryPointTable) {
    for (const auto& pair : childHistoryPointTable) {
      const auto& point = pair.second;
      memoryRef(point.pc, pair.first, point.isWrite, point.numAccesses);

      auto p = point.next.get();
      while (p) {
        memoryRef(p->pc, pair.first, p->isWrite, p->numAccesses);
        p = p->next.get();
      }
    }
  }

}
