#include "StrideLoop.h"

namespace analyser {

  StrideLoop::StrideLoop() : Loop(), parent(nullptr) {}
  StrideLoop::StrideLoop(StrideLoop* p) : Loop(), parent(p) {}

  // Find and report data dependences, merge pending tables into history tables and flush the
  // pending tables and killed addresses.
  void StrideLoop::iterate() {
    if (iter == 0) {
      // No need to do the dependence check on the first iteration.
      std::swap(historyPointTable, pendingPointTable);
      std::swap(historyStrideTable, pendingStrideTable);

    } else {
      // Stride-stride dependences
      // TODO woefully inefficient, use interval tree
      for (const auto& pair : pendingStrideTable) {
        for (const auto& historyPair : historyStrideTable) {
          for (const auto& stride : pair.second) {
            for (const auto& historyStride : historyPair.second) {
              if ((stride.isWrite || historyStride.isWrite) && stride.numDependences(historyStride)) {
                reportDependence(historyPair.first, pair.first, historyStride.isWrite,
                                 stride.isWrite, historyStride.iterLastAccessed,
                                 stride.iterLastAccessed);
              }
            }
          }
        }
      }

      // Point-stride
      // TODO woefully inefficient, use interval tree
      for (const auto& pair : pendingPointTable) {
        for (const auto& historyPair : historyStrideTable) {
          for (const auto& historyStride : historyPair.second) {
            if (historyStride.isDependent(pair.first)) {
              for (const auto& point : pair.second) {
                reportDependence(historyPair.first, pair.first, historyStride.isWrite,
                                 point.isWrite, historyStride.iterLastAccessed,
                                 point.iterLastAccessed);
              }
            }
          }
        }
      }

      // Stride-point dependences
      // TODO woefully inefficient, use interval tree
      for (const auto& pair : pendingStrideTable) {
        for (const auto& historyPair : historyPointTable) {
          for (const auto& stride : pair.second) {
            if (stride.isDependent(historyPair.first)) {
              for (const auto& historyPoint : historyPair.second) {
                reportDependence(historyPair.first, pair.first, historyPoint.isWrite,
                                 stride.isWrite, historyPoint.iterLastAccessed,
                                 stride.iterLastAccessed);
              }
            }
          }
        }
      }

      // Point-point dependences and merge point tables
      dependenceCheckAndMerge();

      // Merge stride tables
      mergeStrideTables();

      pendingPointTable.clear();
      pendingStrideTable.clear();
    }

    killedAddrs.clear();

    ++iter;
  }

  // To handle loop nests, propagate the history tables of L to the parent of L, if it exists.
  void StrideLoop::terminate() {
    iterate();
    if (parent) {
      parent->propagate(historyPointTable, historyStrideTable, killedAddrs);
    }
  }

  // On a memory access, R, check the killed bit of R. If not killed, store R in either
  // pendingPointTable or pendingStrideTable based on the result of the stride detection of R. If R
  // is a write, set its killed bit.
  void StrideLoop::memoryRef(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses) {
    // Check for loop independent dependences.
    if (pendingPointTable.count(addr)) {
      for (const auto& point : pendingPointTable.at(addr)) {
        if (point.isWrite || isWrite) {
          reportIndependentDependence(point.pc, pc, point.isWrite, isWrite);
        }
      }
    }

    // TODO inefficient, use interval tree
    for (const auto& pair : pendingStrideTable) {
      for (const auto& stride : pair.second) {
        if (stride.isDependent(addr)) {
          reportIndependentDependence(pair.first, pc, stride.isWrite, isWrite);
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

  // Propagation is done by merging the history tables of L with the pending tables of the parent
  // of L.
  // To handle loop-independent dependences, if a memory address in the history tables of L is
  // killed by the parent of L, this killed history is not propagated.
  void StrideLoop::propagate(const PointTableT& childPointTable, StrideTableT& childStrideTable,
                             const std::set<uint64_t>& childKilledAddrs) {
    for (auto& pair : childStrideTable) {
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

    Loop::propagate(childPointTable, childKilledAddrs);
  }

  void StrideLoop::mergeStrideTables() {
    for (const auto& pair : pendingStrideTable) {
      for (const auto& stride : pair.second) {
        mergeStride(historyStrideTable[pair.first], stride);
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
        s.iterLastAccessed = toMerge.iterLastAccessed;
        s.numAccesses += toMerge.numAccesses;
        merged = true;
        break;
      }
    }

    if (!merged) {
      strides.push_back(toMerge);
    }
  }

}
