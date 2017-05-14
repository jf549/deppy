#include "StrideLoop.h"
#include <lib/BoundedBuffer.hpp>
#include <lib/event.h>

#include <boost/program_options.hpp>

#include <unistd.h>
#include <array>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stack>
#include <thread>

#ifdef BENCHMARK
#include <chrono>
#include <ctime>
#endif

#define BUFLEN (32 * BUFSIZ)

static unsigned NUM_THREADS = 1;

using namespace analyser;

template<typename T> using Buf = lib::BoundedBuffer<T>;
template<typename T> using Bufs = std::vector<Buf<T>>;
using MemEventT = std::pair<PcT, AddrT>;

int eventDispatch(Bufs<event_t>& eventBufs, Bufs<MemEventT>& memEventBufs);
void eventHandler(Buf<event_t>& eventBuf, Buf<MemEventT>& memEventBuf, bool detailedResults,
                  bool useStrides);
int sd3(bool detailedResults, bool useStrides);

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

          {
            auto thread = (addr >> 7) % NUM_THREADS;
            eventBufs[thread].produce(event);
            memEventBufs[thread].produce({ pc, addr });
          }

          break;

        case SENTINAL:
          break;
      }
    }
  }

  return 0;
}

void eventHandler(Buf<event_t>& eventBuf, Buf<MemEventT>& memEventBuf, bool detailedResults,
                  bool useStrides) {
  PcT pc;
  AddrT addr;
  event_t event;
  std::stack<std::unique_ptr<Loop>> loopStack;
  std::unique_ptr<DependenceResults> results(detailedResults
    ? std::unique_ptr<DependenceResults>{ std::make_unique<DetailedDependenceResults>() }
    : std::unique_ptr<DependenceResults>{ std::make_unique<BasicDependenceResults>() } );

  while (true) {
    event = eventBuf.consume();

    switch (event) {
      case LOOP_ENTRY:
        if (loopStack.empty()) {
          auto ptr = useStrides
            ? std::make_unique<StrideLoop>(detailedResults)
            : std::make_unique<PointLoop>(detailedResults);
          loopStack.push(std::move(ptr)); // Top level loop

        } else {
          auto ptr = useStrides
            ? std::make_unique<StrideLoop>(static_cast<StrideLoop*>(loopStack.top().get()),
                                           detailedResults)
            : std::make_unique<PointLoop>(static_cast<PointLoop*>(loopStack.top().get()),
                                          detailedResults);
          loopStack.push(std::move(ptr)); // Nested loop
        }
        break;

      case LOOP_ITER:
        if (!loopStack.empty()) {
          loopStack.top()->iterate();
        }
        break;

      case LOOP_EXIT:
        if (!loopStack.empty()) {
          results->merge(loopStack.top()->terminate());
          loopStack.pop();
        }
        break;

      case LOAD:
      case STORE:
        std::tie(pc, addr) = memEventBuf.consume();

        if (!loopStack.empty()) {
          loopStack.top()->memoryRef(pc, addr, event == STORE);
        }
        break;

      case SENTINAL:
        results->print();
        return;
    }
  }
}

int sd3(bool detailedResults, bool useStrides) {
  uint64_t pc, addr;
  ssize_t num;
  event_t event;
  std::stack<std::unique_ptr<Loop>> loopStack;
  std::array<char, BUFLEN> buf;
  std::unique_ptr<DependenceResults> results(detailedResults
    ? std::unique_ptr<DependenceResults>{ std::make_unique<DetailedDependenceResults>() }
    : std::unique_ptr<DependenceResults>{ std::make_unique<BasicDependenceResults>() } );
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
            auto ptr = useStrides
              ? std::make_unique<StrideLoop>(detailedResults)
              : std::make_unique<PointLoop>(detailedResults);
            loopStack.push(std::move(ptr)); // Top level loop

          } else {
            auto ptr = useStrides
              ? std::make_unique<StrideLoop>(static_cast<StrideLoop*>(loopStack.top().get()),
                                             detailedResults)
              : std::make_unique<PointLoop>(static_cast<PointLoop*>(loopStack.top().get()),
                                            detailedResults);
            loopStack.push(std::move(ptr)); // Nested loop
          }
          break;

        case LOOP_ITER:
          if (!loopStack.empty()) {
            loopStack.top()->iterate();
          }
          break;

        case LOOP_EXIT:
          if (!loopStack.empty()) {
            results->merge(loopStack.top()->terminate());
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
            loopStack.top()->memoryRef(pc, addr, event == STORE);
          }
          break;

        case SENTINAL:
          break;
      }
    }
  }

  results->print();

  return 0;
}

int main(int argc, const char** argv) {
#ifdef BENCHMARK
  auto cStart = std::clock();
  auto tStart = std::chrono::steady_clock::now();
#endif

  int res = 0;
  bool useStrides = false;
  bool detailedResults = false;

  boost::program_options::options_description desc("Options");
  desc.add_options()
    ("help,h", "Print help message")
    ("compress,c", "Enable compression")
    ("detail,d", "Print detailed dependence information")
    ("threads,t", boost::program_options::value<unsigned>(), "Set number of threads");

  boost::program_options::variables_map vm;

  try {
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  } catch (boost::program_options::unknown_option&) {
    std::cout << desc;
    return 1;
  }

  boost::program_options::notify(vm);

  if (vm.count("help")) {
    std::cout << desc;
    return 1;
  }

  if (vm.count("compress")) {
    useStrides = true;
  }

  if (vm.count("detail")) {
    detailedResults = true;
  }

  if (vm.count("threads")) {
    NUM_THREADS = vm["threads"].as<unsigned>();
  }

  std::cerr << (useStrides ? "Compressing memory access\n" : "Not compressesing memory access\n");
  std::cerr << "Running with " << NUM_THREADS << " thread" << (NUM_THREADS > 1 ? "s\n" : "\n");

  if (NUM_THREADS > 1) {
    Bufs<event_t> eventBufs(NUM_THREADS);
    Bufs<MemEventT> memEventBufs(NUM_THREADS);
    std::vector<std::thread> threads;

    for (unsigned i = 0; i < NUM_THREADS; ++i) {
      threads.emplace_back(eventHandler, std::ref(eventBufs[i]), std::ref(memEventBufs[i]),
                           detailedResults, useStrides);
    }

    res = eventDispatch(eventBufs, memEventBufs);

    for (auto& buf : eventBufs) {
      buf.produce(SENTINAL);
    }

    for (auto& t : threads) {
      t.join();
    }

  } else {
    res = sd3(detailedResults, useStrides);
  }

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
