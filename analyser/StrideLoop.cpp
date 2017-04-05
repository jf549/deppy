#include "StrideLoop.h"

#include <iostream>

namespace analyser {

  StrideLoop::StrideLoop() : Loop(), parent(nullptr) {}
  StrideLoop::StrideLoop(StrideLoop* p) : Loop(), parent(p) {}

  // At the end of the iteration, check data dependences. Also, perform stride-based dependence
  // checking. Report any found data dependences.
  // Merge PendingPointTable with HistoryPointTable. Also, merge the stride tables.
  // The pending tables including killed bits are flushed.
  void StrideLoop::iterate() {
    Loop::iterate(); // Point-point dependences
    // TODO Point-Stride and Stride-Stride
  }

  // To handle loop nests, propagate the history tables of L to the parent of L, if it exists.
  void StrideLoop::terminate() {
    iterate();
    if (parent) {
      parent->propagate(historyPointTable, historyStrideTable);
    }
  }

  // On a memory access, R, check the killed bit of R.
  // If killed, report a loop-independent dependence.
  // Otherwise, store R in either PendingPointTable or PendingStrideTable based on the result of the
  // stride detection of R.
  // Finally, if R is a write, set its killed bit.
  // TODO report loop independent dependences
  void StrideLoop::memoryRef(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses) {
    auto& detector = detectors[pc]; // Default constructs detector if doesn't exist

    if (detector.addAddress(addr)) {
      if (!killedAddrs.count(addr)) {
        if (pendingStrideTable.count(pc)) {
          auto& strides = pendingStrideTable.at(pc);
          strides.emplace_back(
            Stride{ addr, detector.getStride(), addr, isWrite, numAccesses, iter });

        } else {
          pendingStrideTable.emplace(pc, std::vector<Stride>{
            { addr, detector.getStride(), addr, isWrite, numAccesses, iter } });
        }

        if (isWrite) {
          killedAddrs.insert(addr);
        }
      }

    } else {
      Loop::memoryRef(pc, addr, isWrite, numAccesses);
    }
  }

  // Propagation is done by merging the history tables of L with the pending tables of the parent
  // of L.
  // To handle loop-independent dependences, if a memory address in the history tables of L is
  // killed by the parent of L, this killed history is not propagated.
  void StrideLoop::propagate(const PointTableT& childPointTable,
                             const StrideTableT& childStrideTable) {
    Loop::propagate(childPointTable);
  }

}
