#ifndef EVENT_H
#define EVENT_H

#include <cstdint>
#include <utility>

namespace analyser {

  struct LoopEvent {
    enum class Type { loopStart, loopIter, loopEnd } type;
  };

  struct MemoryEvent {
    uint64_t pc;
    uint64_t addr;
    bool isWrite;
  };

  struct Event {
    Event(LoopEvent&& e) : id(ID::loopEvent), le(std::move(e)) {}
    Event(MemoryEvent&& e) : id(ID::memoryEvent), me(std::move(e)) {}

    Event(const LoopEvent& e) : id(ID::loopEvent), le(e) {}
    Event(const MemoryEvent& e) : id(ID::memoryEvent), me(e) {}

    enum class ID { loopEvent, memoryEvent } id;

    union {
      LoopEvent le;
      MemoryEvent me;
    };
  };

}

#endif // EVENT_H
