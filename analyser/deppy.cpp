#include "StrideLoop.h"
#include <tracer/tracerdefs.h>

#include <iostream>
#include <stack>

using namespace analyser;

int main() {
  uint64_t pc;
  uint64_t addr;
  uint8_t eventId;
  std::stack<StrideLoop> loopStack;

  while (std::cin.read((char*)&eventId, sizeof(eventId))) {
    switch (eventId) {
      case LOOP_ENTRY:
        if (loopStack.empty()) {
          loopStack.emplace(); // Top level loop
        } else {
          loopStack.emplace(&loopStack.top()); // Nested loop
        }
        break;

      case LOOP_ITER:
        if (!loopStack.empty()) {
          loopStack.top().iterate();
        }
        break;

      case LOOP_EXIT:
        if (!loopStack.empty()) {
          loopStack.top().terminate();
          loopStack.pop();
        }
        break;

      case LOAD:
      case STORE:
        std::cin.read((char*)&pc, sizeof(pc));
        std::cin.read((char*)&addr, sizeof(addr));
        if (!loopStack.empty()) {
          loopStack.top().memoryRef(pc, addr, eventId == STORE);
        }
        break;

      default:
        std::cerr << "Invalid event\n";
        return 1;
    }
  }
  // std::queue<Event> q;
  // q.push({ { LoopEvent::Type::loopStart } });
  // q.push({ { 1, 1, false } });
  // q.push({ { 2, 1, true } });
  // q.push({ { LoopEvent::Type::loopIter } });
  // q.push({ { 1, 1, false } });
  // q.push({ { LoopEvent::Type::loopIter } });
  // q.push({ { 1, 1, false } });
  // q.push({ { LoopEvent::Type::loopEnd } });
  // sd3(q);
}
