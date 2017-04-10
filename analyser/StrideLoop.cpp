#include "StrideLoop.h"
#include "Logger.h"

namespace analyser {

  StrideLoop::StrideLoop() : PointLoop(), parent(nullptr) {}

  StrideLoop::StrideLoop(StrideLoop* p) : PointLoop(), parent(p) {}

  // Merge the history tables of childLoop with the pending tables of this loop (its parent). To
  // handle loop-independent dependences, if a memory address in the history tables of childLoop is
  // killed by the parent of L, this killed history is not propagated.
  void StrideLoop::propagate(StrideLoop& childLoop) {
    for (auto& pair : childLoop.historyStrideTable) {
      auto& strides = pair.second;

      for (auto& stride : strides) {
        auto eliminated = false;

        for (const auto addr : killedAddrs) {
          if (stride.isDependent(addr)) {
            if (stride.base == stride.limit) {
              eliminated = true;
              break;

            } else if (addr == stride.base) {
              stride.base += stride.stride;

            } else if (addr == stride.limit) {
              stride.limit -= stride.stride;

            } else {
              strides.emplace_back(Stride{ addr + stride.stride, stride.stride, stride.limit,
                                   stride.isWrite, stride.numAccesses, stride.iterLastAccessed });
              stride.limit = addr - stride.stride;
            }
          }
        }

        if (!eliminated) {
          pendingStrideTable[pair.first].emplace_back(std::move(stride));
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
    bool merged = false;

    for (auto& s : strides) {
      if (toMerge.isWrite == s.isWrite && toMerge.stride == s.stride
          && toMerge.base >= s.base - s.stride && toMerge.limit <= s.limit + s.stride) {
        s.base = std::min(s.base, toMerge.base);
        s.limit = std::max(s.limit, toMerge.limit);
        s.iterLastAccessed = iter;
        s.numAccesses += toMerge.numAccesses;
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

  void StrideLoop::doPropagation() {
    if (parent) {
      parent->propagate(*this);
    }
  }

  // On a memory access, R, check the killed bit of R. If not killed, store R in either
  // pendingPointTable or pendingStrideTable based on the result of the stride detection of R. If R
  // is a write, set its killed bit.
  void StrideLoop::addMemoryRef(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses) {
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
        pendingStrideTable[pc].emplace_back(
          Stride{ addr, detector.getStride(), addr, isWrite, numAccesses, iter });

      } else { // R is a point.
        pendingPointTable[addr].emplace_back(Point{ pc, isWrite, numAccesses, iter });
      }

      if (isWrite) {
        killedAddrs.insert(addr);
      }
    }
  }

}
