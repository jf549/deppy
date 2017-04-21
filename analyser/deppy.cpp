#include "StrideLoop.h"
#include <lib/event.h>

#include <iostream>
#include <stack>

using namespace analyser;

int main() {
  mem_event_t memEvent;
  event_t event;
  std::stack<StrideLoop> loopStack;

  while (std::cin.read(reinterpret_cast<char*>(&event), sizeof(event))) {
    switch (event) {
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
        std::cin.read(reinterpret_cast<char*>(&memEvent), sizeof(memEvent));
        if (!loopStack.empty()) {
          loopStack.top().memoryRef(memEvent.pc, memEvent.addr, event == STORE);
        }
        break;
    }
  }
}
