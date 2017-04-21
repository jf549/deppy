#ifndef LOOP_H
#define LOOP_H

#include <cstdint>

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
    void memoryRef(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses = 1);

  private:
    // Using NVI design pattern.
    virtual void doDependenceCheck() = 0;
    virtual void doIteration() = 0;
    virtual void doPropagation() = 0;
    virtual void addMemoryRef(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses) = 0;
  };

}

#endif // LOOP_H
