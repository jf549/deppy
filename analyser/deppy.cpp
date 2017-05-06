#include "StrideLoop.h"
#include <lib/BoundedBuffer.hpp>
#include <lib/event.h>

#include <unistd.h>
#include <array>
#include <iostream>
#include <iomanip>
#include <stack>
#include <thread>

#ifdef BENCHMARK
#include <chrono>
#include <ctime>
#endif

#define BUFLEN (32 * BUFSIZ)
#define NUM_THREADS 2

using namespace analyser;

template<typename T> using Buf = lib::BoundedBuffer<T>;
template<typename T> using Bufs = std::array<Buf<T>, NUM_THREADS>;
using MemEventT = std::pair<PcT, AddrT>;

int eventDispatch(Bufs<event_t>& eventBufs, Bufs<MemEventT>& memEventBufs);
void eventHandler(Buf<event_t>& eventBuf, Buf<MemEventT>& memEventBuf);
int sd3();

int eventDispatch(Bufs<event_t>& eventBufs, Bufs<MemEventT>& memEventBufs) {
  uint64_t pc, addr;
  ssize_t num;
  event_t event;
  std::array<char, BUFLEN> buf;
  const auto front = begin(buf);
  const auto back = cend(buf);

  while ((num = read(STDIN_FILENO, front, buf.size())) > 0) {
    auto it = front; // Pointer to next byte to process
    auto end = front + num; // Pointer to one past the last valid byte

    while (it != end) {
      event = static_cast<event_t>(*it++);

      switch (event) {
        case LOOP_ENTRY:
        case LOOP_ITER:
        case LOOP_EXIT:
          for (auto& eventBuf : eventBufs) {
            eventBuf.produce(event);
          }
          break;

        case LOAD:
        case STORE:
          // If there are fewer remaining bytes in buffer than the size of a memory event, then
          // first copy remaining bytes at the end of the buffer to the front and then refill the
          // buffer by reading from stdin. A circular buffer would provide better performance, but
          // IO is far from a bottleneck in the profiler, so the simple solution wins.
          if (end - it < 16) {
            end = std::copy(it, end, front);
            it = front;

            do {
              num = read(STDIN_FILENO, end, static_cast<size_t>(back - end));

              if (num <= 0) {
                std::cerr << "Did not receive memory event\n";
                return 1;
              }

              end += num;

            } while (end - it < 16);
          }

          memcpy(&pc, it, sizeof(pc));
          it += sizeof(pc);

          memcpy(&addr, it, sizeof(addr));
          it += sizeof(addr);

          auto thread = (addr >> 7) % NUM_THREADS;
          eventBufs[thread].produce(event);
          memEventBufs[thread].produce({ pc, addr });
      }
    }
  }

  return 0;
}

void eventHandler(Buf<event_t>& eventBuf, Buf<MemEventT>& memEventBuf) {
  PcT pc;
  AddrT addr;
  event_t event;
  std::stack<StrideLoop> loopStack;

  while (true) {
    event = eventBuf.consume();

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
        std::tie(pc, addr) = memEventBuf.consume();

        if (!loopStack.empty()) {
          loopStack.top().memoryRef(pc, addr, event == STORE);
        }
        break;

      case SENTINAL:
        return;
    }
  }
}

int sd3() {
  uint64_t pc, addr;
  ssize_t num;
  event_t event;
  std::stack<StrideLoop> loopStack;
  std::array<char, BUFLEN> buf;
  const auto front = begin(buf);
  const auto back = cend(buf);

  while ((num = read(STDIN_FILENO, front, buf.size())) > 0) {
    auto it = front; // Pointer to next byte to process
    auto end = front + num; // Pointer to one past the last valid byte

    while (it != end) {
      event = static_cast<event_t>(*it++);

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
          // If there are fewer remaining bytes in buffer than the size of a memory event, then
          // first copy remaining bytes at the end of the buffer to the front and then refill the
          // buffer by reading from stdin. A circular buffer would provide better performance, but
          // IO is far from a bottleneck in the profiler, so the simple solution wins.
          if (end - it < 16) {
            end = std::copy(it, end, front);
            it = front;

            do {
              num = read(STDIN_FILENO, end, static_cast<size_t>(back - end));

              if (num <= 0) {
                std::cerr << "Did not receive memory event\n";
                return 1;
              }

              end += num;

            } while (end - it < 16);
          }

          memcpy(&pc, it, sizeof(pc));
          it += sizeof(pc);

          memcpy(&addr, it, sizeof(addr));
          it += sizeof(addr);

          if (!loopStack.empty()) {
            loopStack.top().memoryRef(pc, addr, event == STORE);
          }
          break;
      }
    }
  }

  return 0;
}

int main() {
#ifdef BENCHMARK
  auto cStart = std::clock();
  auto tStart = std::chrono::steady_clock::now();
#endif

  int res = 0;

#ifdef NUM_THREADS
  Bufs<event_t> eventBufs;
  Bufs<MemEventT> memEventBufs;
  std::vector<std::thread> threads;

  for (size_t i = 0; i < NUM_THREADS; ++i) {
    threads.emplace_back(eventHandler, std::ref(eventBufs[i]), std::ref(memEventBufs[i]));
  }

  res = eventDispatch(eventBufs, memEventBufs);

  for (auto& buf : eventBufs) {
    buf.produce(SENTINAL);
  }

  for (auto& t : threads) {
    t.join();
  }
#else
  res = sd3();
#endif

#ifdef BENCHMARK
  auto cEnd = std::clock();
  auto tEnd = std::chrono::steady_clock::now();
  std::cerr << std::fixed << std::setprecision(2) << "CPU time used: "
            << 1000.0 * (cEnd - cStart) / CLOCKS_PER_SEC << " ms\n"
            << "Time passed: "
            << std::chrono::duration<double, std::milli>(tEnd - tStart).count() << " ms\n";
#endif

  return res;
}
