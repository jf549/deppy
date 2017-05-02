#include "StrideLoop.h"
#include "Logger.h"

#include <list>

namespace analyser {

  StrideLoop::StrideLoop() : PointLoop(), parent(nullptr) {}

  StrideLoop::StrideLoop(StrideLoop* p) : PointLoop(), parent(p) {}

  // Merge the history tables of childLoop with the pending tables of this loop (its parent). To
  // handle loop-independent dependences, if a memory address in the history tables of childLoop is
  // killed by the parent of L, this killed history is not propagated.
  // TODO use interval tree
  void StrideLoop::propagate(const StrideLoop& childLoop) {
    for (const auto& pair : childLoop.historyStrideTable) {
      for (const auto& s : pair.second) {
        std::list<Stride> worklist{ s };

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
                                           stride.numAccesses, stride.iterLastAccessed,
                                           stride.isWrite });
                stride.limit = addr - stride.stride;
              }
            }

            ++it;
          }
        }

        if (worklist.size()) {
          auto& strides = pendingStrideTable[pair.first];
          strides.insert(cbegin(strides), cbegin(worklist), cend(worklist));
        }
      }
    }

    PointLoop::propagate(childLoop);
  }

  // TODO improve efficiency of these three functions by using interval tree.
  void StrideLoop::findStrideStrideDependences() {
    for (const auto& pair : pendingStrideTable) {
      for (const auto& historyPair : historyStrideTable) {
        for (const auto& stride : pair.second) {
          for (const auto& historyStride : historyPair.second) {
            if ((stride.isWrite || historyStride.isWrite) && stride.numDependences(historyStride)) {
              reportDependence(historyPair.first, pair.first, historyStride.isWrite,
                stride.isWrite, historyStride.iterLastAccessed, iter);
            }
          }
        }
      }
    }
  }

  void StrideLoop::findStridePointDependences() {
    for (const auto& pair : pendingPointTable) {
      for (const auto& historyPair : historyStrideTable) {
        for (const auto& historyStride : historyPair.second) {
          if (historyStride.isDependent(pair.first)) {
            for (const auto& point : pair.second) {
              if (point.isWrite || historyStride.isWrite) {
                reportDependence(historyPair.first, pair.first, historyStride.isWrite,
                  point.isWrite, historyStride.iterLastAccessed, iter);
              }
            }
          }
        }
      }
    }
  }

  void StrideLoop::findPointStrideDependences() {
    for (const auto& pair : pendingStrideTable) {
      for (const auto& historyPair : historyPointTable) {
        for (const auto& stride : pair.second) {
          if (stride.isDependent(historyPair.first)) {
            for (const auto& historyPoint : historyPair.second) {
              if (stride.isWrite || historyPoint.isWrite) {
                reportDependence(historyPair.first, pair.first, historyPoint.isWrite,
                  stride.isWrite, historyPoint.iterLastAccessed, iter);
              }
            }
          }
        }
      }
    }
  }

  void StrideLoop::mergeStride(std::vector<Stride>& strides, const Stride& toMerge) const {
    auto merged = false;

    for (auto& s : strides) {
      if (toMerge.base >= s.base - s.stride && toMerge.limit <= s.limit + s.stride
          && s.merge(toMerge)) {
        merged = true;
        break;
      }
    }

    if (!merged) {
      strides.push_back(toMerge);
    }
  }

  void StrideLoop::mergeStrideTables() {
    for (const auto& pair : pendingStrideTable) {
      for (const auto& stride : pair.second) {
        mergeStride(historyStrideTable[pair.first], stride);
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
  }

  void StrideLoop::doPropagation() const {
    if (parent) {
      parent->propagate(*this);
    }
  }

  // On a memory access, R, check the killed bit of R. If not killed, store R in either
  // pendingPointTable or pendingStrideTable based on the result of the stride detection of R. If R
  // is a write, set its killed bit.
  void StrideLoop::addMemoryRef(uint64_t pc, uint64_t addr, bool isWrite) {
    // Check for loop independent dependences.
    if (pendingPointTable.count(addr)) {
      for (const auto& point : pendingPointTable.at(addr)) {
        if (point.isWrite || isWrite) {
          reportIndependentDependence(point.pc, pc, point.isWrite, isWrite, iter);
        }
      }
    }

    // TODO inefficient, use interval tree
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
          Stride{ addr, detector.getStride(), addr, 1, iter, isWrite });

      } else { // R is a point.
        pendingPointTable[addr].push_back(Point{ pc, iter, isWrite });
      }

      if (isWrite) {
        killedAddrs.insert(addr);
      }
    }
  }

}
