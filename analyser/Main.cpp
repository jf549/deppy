#include "StrideDetector.h"
#include "DynamicGcd.h"

#include <stack>
#include <map>
#include <memory>

using namespace analyser;

struct Event {
  virtual ~Event() {}
};

struct LoopEvent : public Event {
  enum class Type {loopStart, loopIter, loopEnd} type;
};

struct MemoryEvent : public Event {
  unsigned int pc;
  uint64_t addr;
  bool isWrite;
};

struct LoopInstance {
  LoopInstance() : iter(0) {}
  unsigned int iter;
};

struct Point {
  unsigned int pc;
  unsigned int numAccesses;
  Point *next;
};

struct Stride2 {
  uint64_t base, stride, limit;
  unsigned int numAccesses;
  Stride2 *next;
};

void sd3(Event *queue[], unsigned int len) {
  std::map<uint64_t, Point> pendingPointTable, historyPointTable;
  std::map<unsigned int, Stride> pendingStrideTable, historyStrideTable;
  std::stack<LoopInstance> loopStack;
  std::map<unsigned int, std::unique_ptr<StrideDetector>> detectors;

  for (int i = 0; i < len; ++i) {
    Event *e = queue[i];

    if (LoopEvent *le = dynamic_cast<LoopEvent*>(e)) {
      if (le->type == LoopEvent::Type::loopStart) {
        // 1: When a loop, L, starts, LoopInstance of L is pushed on LoopStack.
        loopStack.emplace();

      } else if (le->type == LoopEvent::Type::loopIter) {
        // 3: At the end of the iteration, do the stride-based dependence checking (Sections 4.2 and 4.4).
        // Report any found dependences.

        // 4: After Step 3, merge the pending and history point tables. Also, merge the stride tables
        // (Section 4.5). Finally, the pending tables, including killed bits, are flushed.

      } else if (le->type == LoopEvent::Type::loopEnd) {
        // 5: When L terminates, flush the history tables, and pop the LoopStack. To handle loop nests, we
        // propagate the history tables of L to the parent of L, if they exist. Propagation is done by
        // merging the history tables of L with the pending tables of the parent of L.

        // Meanwhile, to handle loop-independent dependences, if a memory address in the history tables of L
        // is killed by the parent of L (Section 4.6), this killed history is not propagated.
      }

    } else if (MemoryEvent *me = dynamic_cast<MemoryEvent*>(e)) {
      // 2: On a memory reference, R, of L’s i-th iteration, check the killed bit of R. If killed, report
      // a loop-independent dependence, and halt the following steps.

      // Otherwise, store R in either PendingPointTable or PendingStrideTable based on the result of the
      // stride detection (Section 4.1). If R is a write, set its killed bit.

      if (false) { // TODO check killed bit
        printf("Loop-independent dependence %llu", me->addr);
      } else {
        detectors[me->pc]->addAddress(me->addr);
      }

    } else {
      perror("Error: unknown event");
    }
  }
}

int main(void) {
  Event *queue[4];
  sd3(queue, 4);
}
