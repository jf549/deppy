#include "StrideDetector.h"

#include <iostream>
#include <array>

using namespace analyser;

StrideDetector::StrideDetector() : state(State::start) {}

void StrideDetector::addAddress(uint64_t pc, uint64_t addr, bool isWrite, unsigned int numAccesses) {
  switch (state) {
    case State::start:
      stride = { addr, 0, 0, 0 };
      state = State::firstObs;
      break;

    case State::firstObs:
      stride.stride = addr - stride.base;
      stride.limit = addr;
      state = State::strideLnd;
      break;

    case State::strideLnd:
      // addr = base + stride * n for some n
      // addr - base = stride * n for some n
      // (addr - base) / stride = n for some n in N
      if ((addr - stride.base) % stride.stride == 0) {
        if (stride.limit < addr) {
          stride.limit = addr;
        } // TODO: deal with negative n
        state = State::weakStride;
      } else {
        state = State::firstObs;
      }
      break;

    case State::weakStride:
      if ((addr - stride.base) % stride.stride == 0) {
        if (stride.limit < addr) {
          stride.limit = addr;
        } // TODO: deal with negative n
        state = State::strongStride;
      } else {
        state = State::strideLnd;
      }
      break;

    case State::strongStride :
      if ((addr - stride.base) % stride.stride == 0) {
        if (stride.limit < addr) {
          stride.limit = addr;
        } // TODO: deal with negative n
      } else {
        state = State::weakStride;
      }
      break;
  }
}

// int main() {
//   std::array<uint64_t, 10> a{ { 10, 14, 18, 14, 18, 22, 30, 18, 22, 26 } };
//   StrideDetector sd;
//   for (auto i : a) {
//     sd.addAddress(i);
//   }
//   std::cout << sd.getStride().base << std::endl;
//   std::cout << sd.getStride().stride << std::endl;
//   std::cout << sd.getStride().limit << std::endl;
// }
