#include "Event.h"
#include "Loop.h"

#include <stack>
#include <queue>
#include <iostream>

using namespace analyser;

void sd3(std::queue<Event>& q) {
  std::stack<Loop> loopStack;

  while (!q.empty()) {
    auto& event = q.front();
    q.pop();

    if (event.id == Event::ID::loopEvent) {
      switch (event.le.type) {
        case LoopEvent::Type::loopStart:
          // When a loop, L, starts, LoopInstance of L is pushed on LoopStack.
          if (loopStack.empty()) {
            loopStack.emplace();
          } else {
            loopStack.emplace(&loopStack.top());
          }
          break;

        case LoopEvent::Type::loopIter:
          loopStack.top().iterate();
          break;

        case LoopEvent::Type::loopEnd:
          loopStack.top().terminate();
          loopStack.pop();
          break;
      }

    } else { // event.id == Event::ID::memoryEvent
      loopStack.top().memoryRef(event.me.pc, event.me.addr, event.me.isWrite);
    }
  }
}

int main() {
  std::queue<Event> q;
  q.push({ { LoopEvent::Type::loopStart } });
  q.push({ { 1, 1, true } });
  q.push({ { 2, 1, true } });
  q.push({ { LoopEvent::Type::loopIter } });
  q.push({ { 1, 1, true } });
  q.push({ { LoopEvent::Type::loopEnd } });
  sd3(q);
}
