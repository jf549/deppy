#ifndef LOOP_H
#define LOOP_H

#include "types.h"

namespace analyser {

  class Loop {
  public:
    Loop();
    virtual ~Loop();

    // Call each time a LoopIter event is seen for this loop.
    void iterate();

    // Call when the LoopEnd event is seen for this loop.
    void terminate();

    // Call each time a memory access event is seen inside this loop.
    void memoryRef(PcT pc, AddrT addr, bool isWrite);

  private:
    // Using NVI design pattern.
    virtual void doDependenceCheck() = 0;
    virtual void doIteration() = 0;
    virtual void doPropagation() const = 0;
    virtual void addMemoryRef(PcT pc, AddrT addr, bool isWrite) = 0;
  };

}

#endif // LOOP_H
