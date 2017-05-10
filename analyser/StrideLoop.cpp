#include "StrideLoop.h"
#include "Logger.h"

namespace analyser {

  StrideLoop::StrideLoop() : PointLoop(), parent(nullptr) {}

  StrideLoop::StrideLoop(StrideLoop* p) : PointLoop(), parent(p) {}

  // Merge the history tables of childLoop with the pending tables of this loop (its parent). To
  // handle loop-independent dependences, if a memory address in the history tables of childLoop is
  // killed by the parent of L, this killed history is not propagated.
  void StrideLoop::propagate(const StrideLoop& childLoop) {
    for (const auto& pair : childLoop.historyStrideTable) {
      auto& strideList = pendingStrideTable[pair.first];
      std::vector<IntervalT> intervals;

      for (auto& stride : strideList) {
        intervals.push_back(IntervalT(stride.base, stride.limit, &stride));
      }

      IntervalTreeT tree(intervals);

      for (const auto& s : pair.second) {
        StrideListT worklist{ s };

        for (const auto addr : killedAddrs) {
          auto it = begin(worklist);

          while (it != end(worklist)) {
            auto& stride = *it;

            if (stride.isDependent(addr)) {
              if (stride.base == stride.limit) {
                it = worklist.erase(it);
                continue;

              } else if (addr == stride.base) {
                stride.base += stride.stride;

              } else if (addr == stride.limit) {
                stride.limit -= stride.stride;

              } else {
                worklist.push_front(Stride{ addr + stride.stride, stride.stride, stride.limit,
                                            stride.iterLastAccessed, stride.isWrite });
                stride.limit = addr - stride.stride;
              }
            }

            ++it;
          }
        }

        if (worklist.size()) {
          for (const auto& stride : worklist) {
            auto merged = false;
            std::vector<IntervalT> overlapping;

            tree.findOverlapping(stride.base, stride.limit, overlapping);

            for (auto& interval : overlapping) {
              if (interval.value->merge(stride)) {
                merged = true;
                break;
              }
            }

            if (!merged) {
              strideList.push_back(stride);
            }
          }
        }
      }
    }

    PointLoop::propagate(childLoop);
  }

  void StrideLoop::findStrideStrideDependences() const {
    for (const auto& pair : pendingStrideTable) {
      for (const auto& stride : pair.second) {
        std::vector<IntervalT> dependences;
        intervalTree.findOverlapping(stride.base, stride.limit, dependences);

        for (const auto& interval : dependences) {
          const auto historyStride = interval.value;
          if ((stride.isWrite || historyStride->isWrite) && stride.numDependences(*historyStride)) {
            reportDependence(0, pair.first, historyStride->isWrite, stride.isWrite,
                             historyStride->iterLastAccessed, iter);
          }
        }
      }
    }
  }

  void StrideLoop::findStridePointDependences() const {
    for (const auto& pair : pendingPointTable) {
      const auto addr = pair.first;
      std::vector<IntervalT> dependences;
      intervalTree.findOverlapping(addr, addr, dependences);

      for (const auto& interval : dependences) {
        const auto stridePtr = interval.value;

        if (stridePtr->isDependent(addr)) {
          for (const auto& point : pair.second) {
            if (point.isWrite || stridePtr->isWrite) {
              reportDependence(0, point.pc, stridePtr->isWrite, point.isWrite,
                               stridePtr->iterLastAccessed, iter);
            }
          }
        }
      }
    }
  }

  void StrideLoop::findPointStrideDependences() const {
    for (const auto& pair : pendingStrideTable) {
      for (const auto& historyPair : historyPointTable) {
        for (const auto& stride : pair.second) {
          if (stride.isDependent(historyPair.first)) {
            for (const auto& historyPoint : historyPair.second) {
              if (stride.isWrite || historyPoint.isWrite) {
                reportDependence(historyPoint.pc, pair.first, historyPoint.isWrite, stride.isWrite,
                                 historyPoint.iterLastAccessed, iter);
              }
            }
          }
        }
      }
    }
  }

  void StrideLoop::mergeStrideTables() {
    for (const auto& pair : pendingStrideTable) {
      if (historyStrideTable.count(pair.first)) {
        auto& strideList = historyStrideTable.at(pair.first);
        std::vector<IntervalT> intervals;

        for (auto& stride : strideList) {
          intervals.push_back(IntervalT(stride.base, stride.limit, &stride));
        }

        IntervalTreeT tree(intervals);

        for (const auto& stride : pair.second) {
          auto merged = false;
          std::vector<IntervalT> overlapping;

          tree.findOverlapping(stride.base, stride.limit, overlapping);

          for (auto& interval : overlapping) {
            if (interval.value->merge(stride)) {
              merged = true;
              break;
            }
          }

          if (!merged) {
            strideList.push_back(stride);
          }
        }

      } else {
        historyStrideTable.insert(pair);
      }
    }
  }

  // Find and report data dependences, merge pending tables into history tables and flush the
  // pending tables and killed addresses.
  void StrideLoop::doDependenceCheck() {
    if (iter == 0) {
      // No need to do the dependence check on the first iteration.
      std::swap(historyPointTable, pendingPointTable);
      std::swap(historyStrideTable, pendingStrideTable);

    } else {
      findPointPointDependences();
      findStrideStrideDependences();
      findStridePointDependences();
      findPointStrideDependences();

      mergePointTables();
      mergeStrideTables();

      pendingPointTable.clear();
      pendingStrideTable.clear();
    }

    std::vector<IntervalT> intervals;

    for (auto& pair : historyStrideTable) {
      for (auto& stride : pair.second) {
        intervals.push_back(IntervalT(stride.base, stride.limit, &stride));
      }
    }

    intervalTree = IntervalTreeT(intervals);
  }

  void StrideLoop::doPropagation() const {
    if (parent) {
      parent->propagate(*this);
    }
  }

  // On a memory access, R, check the killed bit of R. If not killed, store R in either
  // pendingPointTable or pendingStrideTable based on the result of the stride detection of R. If R
  // is a write, set its killed bit.
  void StrideLoop::addMemoryRef(PcT pc, AddrT addr, bool isWrite) {
    // Check for loop independent dependences.
    if (pendingPointTable.count(addr)) {
      for (const auto& point : pendingPointTable.at(addr)) {
        if (point.isWrite || isWrite) {
          reportIndependentDependence(point.pc, pc, point.isWrite, isWrite, iter);
        }
      }
    }

    for (const auto& pair : pendingStrideTable) {
      for (const auto& stride : pair.second) {
        if (stride.isDependent(addr) && (stride.isWrite || isWrite)) {
          reportIndependentDependence(pair.first, pc, stride.isWrite, isWrite, iter);
        }
      }
    }

    // Insert R into point or stride table.
    if (!killedAddrs.count(addr)) {
      auto& detector = detectors[pc]; // Default constructs detector if it doesn't exist.

      if (detector.addAddress(addr)) { // R is part of a stride.
        pendingStrideTable[pc].push_back(
          Stride{ addr, detector.getStride(), addr, iter, isWrite });

      } else { // R is a point.
        pendingPointTable[addr].push_back(Point{ pc, iter, isWrite });
      }

      if (isWrite) {
        killedAddrs.insert(addr);
      }
    }
  }

}
