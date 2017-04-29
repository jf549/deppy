#include "StrideLoop.h"
#include <lib/event.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <stack>

using namespace analyser;

int main() {
#ifdef BENCHMARK
  auto cStart = std::clock();
  auto tStart = std::chrono::steady_clock::now();
#endif

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
        if (std::cin.read(reinterpret_cast<char*>(&memEvent), sizeof(memEvent))) {
          if (!loopStack.empty()) {
            loopStack.top().memoryRef(memEvent.pc, memEvent.addr, event == STORE);
          }
        } else {
          std::cerr << "Did not receive memory event\n";
          return 1;
        }
        break;
    }
  }

#ifdef BENCHMARK
  auto cEnd = std::clock();
  auto tEnd = std::chrono::steady_clock::now();
  std::cerr << std::fixed << std::setprecision(2) << "CPU time used: "
            << 1000.0 * (cEnd - cStart) / CLOCKS_PER_SEC << " ms\n"
            << "Time passed: "
            << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms\n";
#endif

  return 0;
}
