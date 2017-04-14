#include "PointLoop.h"
#include "Logger.h"

#include <cassert>

namespace analyser {

  PointLoop::PointLoop() : Loop(), iter(0), parent(nullptr) {}

  PointLoop::PointLoop(PointLoop* p) : Loop(), iter(0), parent(p) {}

  // Propagation is done by merging the history table of L with the pending table of the parent of
  // L. To handle loop-independent dependences, if a memory address in the history tables of L is
  // killed by the parent of L, this killed history is not propagated. We essentially treat the
  // inner loop as if it is fully unrolled inside its parent.
  void PointLoop::propagate(const PointLoop& childLoop) {
    for (const auto& pair : childLoop.historyPointTable) {
      const auto addr = pair.first;

      if (!killedAddrs.count(addr)) {
        const auto& childPoints = pair.second;
        auto& points = pendingPointTable[addr];
        points.insert(end(points), begin(childPoints), end(childPoints));
      }
    }

    killedAddrs.insert(begin(childLoop.killedAddrs), end(childLoop.killedAddrs));
  }

  // TODO: try lookup instead
  void PointLoop::findPointPointDependences() {
    auto itp = cbegin(pendingPointTable);
    auto ith = cbegin(historyPointTable);

    while (itp != cend(pendingPointTable) && ith != cend(historyPointTable)) {
      if (itp->first < ith->first) {
        ++itp;

      } else if (ith->first < itp->first) {
        ++ith;

      } else {
        for (const auto& point : itp->second) {
          for (const auto& historyPoint : ith->second) {
            if (historyPoint.isWrite || point.isWrite) {
              reportDependence(historyPoint.pc, point.pc, historyPoint.isWrite, point.isWrite,
                               historyPoint.iterLastAccessed, iter);
            }
          }
        }
        ++itp;
        ++ith;
      }
    }
  }

  void PointLoop::mergePointTables() {
    PointTableT mergedPointTable;
    auto itp = cbegin(pendingPointTable);
    auto endp = cend(pendingPointTable);
    auto ith = cbegin(historyPointTable);
    auto endh = cend(historyPointTable);

    while (true) {
      if (itp == endp) {
        mergedPointTable.insert(ith, endh);
        break;

      } else if (ith == endh) {
        mergedPointTable.insert(itp, endp);
        break;

      } else if (itp->first < ith->first) {
        mergedPointTable.emplace_hint(end(mergedPointTable), std::move(*itp));
        ++itp;

      } else if (ith->first < itp->first) {
        mergedPointTable.emplace_hint(end(mergedPointTable), std::move(*ith));
        ++ith;

      } else {
        auto mergedPoints = std::move(ith->second);

        for (const auto& point : itp->second) {
          bool merged = false;

          for (auto& mergedPoint : mergedPoints) {
            // Merge equivalent accesses occuring in subsequent iterations.
            if (point.pc == mergedPoint.pc) {
              assert(point.isWrite == mergedPoint.isWrite);
              mergedPoint.numAccesses += point.numAccesses;
              mergedPoint.iterLastAccessed = iter;
              merged = true;
              break;
            }
          }

          // If no equivalent access has occurred before, add this one to the history.
          if (!merged) {
            mergedPoints.emplace_back(std::move(point));
          }
        }

        mergedPointTable.emplace_hint(end(mergedPointTable), ith->first, std::move(mergedPoints));
        ++itp;
        ++ith;
      }
    }

    historyPointTable = std::move(mergedPointTable);
  }

  // Do the dependence checking and report any found dependences. Merge the pending and history
  // point tables. Clear the pending point table.
  void PointLoop::doDependenceCheck() {
    if (iter == 0) {
      std::swap(historyPointTable, pendingPointTable);
    } else {
      findPointPointDependences();
      mergePointTables();
      pendingPointTable.clear();
    }
  }

  void PointLoop::doIteration() {
    killedAddrs.clear();
    ++iter;
  }

  // To handle loop nests, propagate the history table of L to the parent of L, if it exists.
  void PointLoop::doPropagation() {
    if (parent) {
      parent->propagate(*this);
    }
  }

  // On a memory reference, R, check the killed bit of R. If killed, report a loop-independent
  // dependence. Otherwise, store R in PendingPointTable. If R is a write, set its killed bit.
  void PointLoop::addMemoryRef(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses) {
    auto& points = pendingPointTable[addr];

    for (const auto& point : points) {
      if (point.isWrite || isWrite) {
        reportIndependentDependence(point.pc, pc, point.isWrite, isWrite, iter);
      }
    }

    if (!killedAddrs.count(addr)) {
      points.emplace_back(Point{ pc, isWrite, numAccesses, iter });

      if (isWrite) {
        killedAddrs.insert(addr);
      }
    }
  }

}
