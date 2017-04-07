#include "Loop.h"

#include <iostream>

namespace analyser {

  Loop::Loop() : iter(0), parent(nullptr) {}
  Loop::Loop(Loop* p) : iter(0), parent(p) {}

  // Do the dependence checking and report any found dependences and merge the pending and history
  // point tables.
  // Finally, the pending table, including killed bits, is flushed.
  void Loop::iterate() {
    if (iter == 0) {
      std::swap(historyPointTable, pendingPointTable);
    } else {
      dependenceCheckAndMerge();
      pendingPointTable.clear();
    }

    killedAddrs.clear();

    ++iter;
  }

  // To handle loop nests, we propagate the history table of L to the parent of L, if it exists.
  void Loop::terminate() {
    iterate(); // Need to check dependences for final iteration
    if (parent) {
      parent->propagate(historyPointTable, killedAddrs);
    }
  }

  // On a memory reference, R, check the killed bit of R.
  // If killed, report a loop-independent dependence.
  // Otherwise, store R in PendingPointTable. If R is a write, set its killed bit.
  void Loop::memoryRef(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses) {
    auto& points = pendingPointTable[addr];

    for (const auto& point : points) {
      if (point.isWrite || isWrite) {
        reportIndependentDependence(point.pc, pc, point.isWrite, isWrite);
      }
    }

    if (!killedAddrs.count(addr)) {
      points.emplace_back(Point{ pc, isWrite, numAccesses, iter });

      if (isWrite) {
        killedAddrs.insert(addr);
      }
    }
  }

  // Propagation is done by merging the history table of L with the pending table of the parent of
  // L. To handle loop-independent dependences, if a memory address in the history tables of L is
  // killed by the parent of L, this killed history is not propagated. We essentially treat the
  // inner loop as if it is fully unrolled inside its parent.
  void Loop::propagate(const PointTableT& childHistoryPointTable,
                       const std::set<uint64_t>& childKilledAddrs) {
    for (const auto& pair : childHistoryPointTable) {
      const auto addr = pair.first;

      if (!killedAddrs.count(addr)) {
        const auto& childPoints = pair.second;
        auto& points = pendingPointTable[addr];
        points.insert(end(points), begin(childPoints), end(childPoints));
      }
    }

    killedAddrs.insert(begin(childKilledAddrs), end(childKilledAddrs));
  }

  void Loop::reportDependence(uint64_t srcPc, uint64_t sinkPc, bool srcIsWrite, bool sinkIsWrite,
                              unsigned int srcIter, unsigned int sinkIter) const {
    std::cout << "Loop-carried dependence: "
              << (srcIsWrite ? (sinkIsWrite ? "WAW" : "RAW") : "WAR")
              << " (line: " << srcPc
              << ", iteration: " << srcIter
              << ") --> (line: " << sinkPc
              << ", iteration: " << sinkIter
              << ")\n";
  }

  void Loop::reportIndependentDependence(uint64_t srcPc, uint64_t sinkPc, bool srcIsWrite,
                                         bool sinkIsWrite) const {
    std::cout << "Loop-independent dependence: "
              << (srcIsWrite ? (sinkIsWrite ? "WAW" : "RAW") : "WAR")
              << " (line: " << srcPc
              << ") --> (line: " << sinkPc
              << ") iteration: " << iter
              << '\n';
  }

  void Loop::dependenceCheckAndMerge() {
    for (const auto& pair : pendingPointTable) {
      const auto addr = pair.first;

      if (historyPointTable.count(addr)) { // Potential dependences
        const auto& points = pair.second;
        auto& historyPoints = historyPointTable.at(addr);

        for (const auto& point : points) {
          bool merged = false;

          for (auto& historyPoint : historyPoints) {
            if (historyPoint.isWrite || point.isWrite) { // Dependence
              reportDependence(historyPoint.pc, point.pc, historyPoint.isWrite, point.isWrite,
                               historyPoint.iterLastAccessed, point.iterLastAccessed);
            }

            // Merge equivalent accesses occuring in subsequent iterations.
            if (point.pc == historyPoint.pc) {
              if (point.isWrite != historyPoint.isWrite) {
                std::cerr << "Bad Point!\n";
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
  }

}
