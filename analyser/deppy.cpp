#include "StrideLoop.h"
#include <lib/event.h>

#include <array>
#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <stack>

#define BUFLEN 4096

using namespace analyser;

int main() {
#ifdef BENCHMARK
  auto cStart = std::clock();
  auto tStart = std::chrono::steady_clock::now();
#endif

  std::stack<StrideLoop> loopStack;
  std::array<char, BUFLEN> buf;
  auto front = begin(buf);
  auto back = cend(buf);

  // Increase IO performance
  std::ios_base::sync_with_stdio(false); // Prevents use of C IO functions
  std::cin.tie(nullptr); // Prevents in/out interaction with console

  while (std::cin.read(front, buf.size()) || std::cin.gcount()) {
    auto it = front;
    auto end = front + std::cin.gcount();

    while (it != end) {
      event_t event = static_cast<event_t>(*it++);

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
          if (end - it < 16) {
            end = std::copy(it, end, front);
            it = front;
            std::cin.read(end, back - end);
            end += std::cin.gcount();
            if (end - it < 16) {
              std::cerr << "Did not receive memory event\n";
              return 1;
            }
          }
          {
            uint64_t pc, addr;
            memcpy(&pc, it, sizeof(pc));
            it += sizeof(pc);
            memcpy(&addr, it, sizeof(addr));
            it += sizeof(addr);
            if (!loopStack.empty()) {
              loopStack.top().memoryRef(pc, addr, event == STORE);
            }
          }
          break;
      }
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
