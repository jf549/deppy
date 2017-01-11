#include "StrideDetector.h"
#include <iostream>

using namespace analyser;

StrideDetector::StrideDetector(int pc, bool isWrite, uint64_t addr) :
  s(State::start), pc(pc), isWrite(isWrite) {
  addAddress(addr);
}

void StrideDetector::addAddress(uint64_t addr) {
  switch (s) {
    case State::start :
      stride = {addr, 0, 0};
      s = State::firstObs;
      break;

    case State::firstObs :
      stride.diff = addr - stride.base;
      stride.limit = 1;
      s = State::strideLnd;
      break;

    case State::strideLnd :
      // addr = base + diff * n for some n
      // addr - base = diff * n for some n
      // (addr - base) / diff = n for some n in N
      if ((addr - stride.base) % stride.diff == 0) {
        uint64_t n = (addr - stride.base) / stride.diff;
        if (stride.limit < n) { stride.limit = n; } // TODO: deal with negative n
        s = State::weakStride;
      } else {
        s = State::firstObs;
      }
      break;

    case State::weakStride :
      if ((addr - stride.base) % stride.diff == 0) {
        uint64_t n = (addr - stride.base) / stride.diff;
        if (stride.limit < n) { stride.limit = n; } // TODO: deal with negative n
        s = State::strongStride;
      } else {
        s = State::strideLnd;
      }
      break;

    case State::strongStride :
      if ((addr - stride.base) % stride.diff == 0) {
        uint64_t n = (addr - stride.base) / stride.diff;
        if (stride.limit < n) { stride.limit = n; } // TODO: deal with negative n
      } else {
        s = State::weakStride;
      }
      break;
  }
}

int main(void) {
  uint64_t a[] = {10, 14, 18, 14, 18, 22, 30, 18, 22, 26};
  StrideDetector sd(0, false, a[0]);
  for (int i = 1; i < 9; ++i) {
    sd.addAddress(a[i]);
  }
  std::cout << sd.getStride().base << std::endl;
  std::cout << sd.getStride().diff << std::endl;
  std::cout << sd.getStride().limit << std::endl;
}
