#include "PointLoop.h"

namespace analyser {

  PointLoop::PointLoop(bool detailedResults) : Loop(detailedResults), iter(0), parent(nullptr) {}

  PointLoop::PointLoop(PointLoop* p, bool detailedResults) : Loop(detailedResults), iter(0), parent(p) {}

  // Propagation is done by merging the history table of L with the pending table of the parent of
  // L. To handle loop-independent dependences, if a memory address in the history tables of L is
  // killed by the parent of L, this killed history is not propagated. We essentially treat the
  // inner loop as if it is fully unrolled inside its parent.
  void PointLoop::propagate(const PointLoop& childLoop) {
    for (const auto& pair : childLoop.historyPointTable) {
      const auto addr = pair.first;
      const auto& childPoints = pair.second;
      auto& points = pendingPointTable[addr];

      for (const auto& childPoint : childPoints) {
        for (const auto& point : points) {
          if (point.isWrite || childPoint.isWrite) {
            results->addIndependentDependence(point.pc, childPoint.pc, point.isWrite,
                                              childPoint.isWrite, iter);
          }
        }
      }

      if (!killedAddrs.count(addr)) {
        points.insert(cend(points), cbegin(childPoints), cend(childPoints));
      }
    }

    killedAddrs.insert(cbegin(childLoop.killedAddrs), cend(childLoop.killedAddrs));
  }

  void PointLoop::findPointPointDependences() const {
    auto itp = cbegin(pendingPointTable);
    auto ith = cbegin(historyPointTable);
    const auto endp = cend(pendingPointTable);
    const auto endh = cend(historyPointTable);

    while (itp != endp && ith != endh) {
      if (itp->first < ith->first) {
        ++itp;

      } else if (ith->first < itp->first) {
        ++ith;

      } else {
        for (const auto& point : itp->second) {
          for (const auto& historyPoint : ith->second) {
            if (historyPoint.isWrite || point.isWrite) {
              results->addCarriedDependence(historyPoint.pc, point.pc, historyPoint.isWrite,
                                            point.isWrite, historyPoint.iterLastAccessed, iter);
            }
          }
        }
        ++itp;
        ++ith;
      }
    }
  }

  void PointLoop::mergePointTables() {
    auto itp = cbegin(pendingPointTable);
    auto ith = begin(historyPointTable);
    const auto endp = cend(pendingPointTable);
    const auto endh = end(historyPointTable);

    while (itp != endp) {
      if (ith == endh) {
        historyPointTable.insert(itp, endp);
        break;

      } else if (itp->first < ith->first) {
        historyPointTable.emplace_hint(ith, std::move(*itp));
        ++itp;

      } else if (ith->first < itp->first) {
        ++ith;

      } else {
        auto& historyPoints = ith->second;

        for (const auto& point : itp->second) {
          auto merged = false;

          for (auto& historyPoint : historyPoints) {
            // Merge equivalent accesses occuring in subsequent iterations.
            if (point.pc == historyPoint.pc) {
              historyPoint.iterLastAccessed = iter;
              merged = true;
              break;
            }
          }

          // If no equivalent access has occurred before, add this one to the history.
          if (!merged) {
            historyPoints.push_back(std::move(point));
          }
        }

        ++itp;
        ++ith;
      }
    }
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
  void PointLoop::doPropagation() const {
    if (parent) {
      parent->propagate(*this);
    }
  }

  // On a memory reference, R, check the killed bit of R. If killed, report a loop-independent
  // dependence. Otherwise, store R in PendingPointTable. If R is a write, set its killed bit.
  void PointLoop::addMemoryRef(PcT pc, AddrT addr, bool isWrite) {
    auto& points = pendingPointTable[addr];

    for (const auto& point : points) {
      if (point.isWrite || isWrite) {
        results->addIndependentDependence(point.pc, pc, point.isWrite, isWrite, iter);
      }
    }

    if (!killedAddrs.count(addr)) {
      points.push_back({ pc, iter, isWrite });

      if (isWrite) {
        killedAddrs.insert(addr);
      }
    }
  }

}
