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

    for (const auto& pair : pendingPointTable) {
      const auto addr = pair.first;

      if (historyPointTable.count(addr)) { // Dependence
        const auto& points = pair.second;
        auto& historyPoints = historyPointTable.at(addr);

        for (const auto& point : points) {
          bool merged = false;

          for (auto& historyPoint : historyPoints) {
            std::cout << "Loop-carried dependence" << '\n'; // TODO details

            // Merge equivalent accesses occuring in subsequent iterations.
            if (point.pc == historyPoint.pc) {
              if (point.isWrite != historyPoint.isWrite) {
                std::cout << "Bad Point!\n";
              }
              historyPoint.numAccesses += point.numAccesses;
              historyPoint.iterLastAccessed = iter;
              merged = true;
            }
          }

          // If no equivalent access has occurred before, add this one to the history.
          if (!merged) {
            historyPoints.emplace_back(std::move(point));
          }
        }

      } else { // No dependence
        historyPointTable.emplace(std::move(pair)); // Merge entire vector for this address
      }
    }

    pendingPointTable.clear();
    killedAddrs.clear();

    ++iter;
  }

  // To handle loop nests, we propagate the history table of L to the parent of L, if it exists.
  void Loop::terminate() {
    iterate(); // Need to check dependences for final iteration
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
        pendingPointTable.at(addr).emplace_back(Point{ pc, isWrite, numAccesses, iter });
        // Each pc can only occur once in an iteration so no need to check for existing point.
      } else {
        pendingPointTable.emplace(addr, std::vector<Point>{ { pc, isWrite, numAccesses, iter } });
      }

      if (isWrite) {
        killedAddrs.insert(addr);
      }
    }
  }

  // Propagation is done by merging the history table of L with the pending table of the parent of
  // L. To handle loop-independent dependences, if a memory address in the history tables of L is
  // killed by the parent of L, this killed history is not propagated. We essential treat the inner
  // loop as if it is fully unrolled inside its parent.
  void Loop::propagate(const PointTableT& childHistoryPointTable) {
    for (const auto& pair : childHistoryPointTable) {
      for (const auto& point : pair.second) {
        memoryRef(point.pc, pair.first, point.isWrite, point.numAccesses);
      }
    }
    //TODO could merge whole vectors in
  }

}
