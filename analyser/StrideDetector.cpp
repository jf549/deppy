#include "StrideDetector.h"

#include <algorithm>

namespace analyser {

  State& operator++(State& state) {
    switch(state) {
      case State::start:
        return state = State::firstObs;

      case State::firstObs:
        return state = State::strideLnd;

      case State::strideLnd:
        return state = State::weakStride;

      case State::weakStride:
      case State::strongStride:
        return state = State::strongStride;
    }
  }

  State operator++(State& state, int) {
    State tmp(state);
    ++state;
    return tmp;
  }

  State& operator--(State& state) {
    switch(state) {
      case State::start:
      case State::firstObs:
        return state = State::start;

      case State::strideLnd:
        return state = State::firstObs;

      case State::weakStride:
        return state = State::strideLnd;

      case State::strongStride:
        return state = State::weakStride;
    }
  }

  State operator--(State& state, int) {
    State tmp(state);
    --state;
    return tmp;
  }

  StrideDetector::StrideDetector() : state(State::start) {}

  bool StrideDetector::addAddress(uint64_t addr) {
    switch (state) {
      case State::start:
        base = addr;
        limit = addr;
        state = State::firstObs;
        return false;

      case State::firstObs:
        base = std::min(base, addr);
        limit = std::max(limit, addr);
        stride = limit - base;
        state = State::strideLnd;
        return false;

      case State::strideLnd:
        if (stride == 0) {
          return false;
        } else if (addr <= limit + stride && addr >= base - stride
                   && (addr > base ? addr - base : base - addr) % stride == 0) {
          base = std::min(base, addr);
          limit = std::max(limit, addr);
          ++state;
          return true;
        } else {
          state = State::start;
          return false;
        }

      case State::weakStride:
      case State::strongStride:
        // Stride iff addr = base + stride * n for some n in N
        // iff (addr - base) / stride = n for some n in N
        if (addr <= limit + stride && addr >= base - stride
            && (addr > base ? addr - base : base - addr) % stride == 0) {
          base = std::min(base, addr);
          limit = std::max(limit, addr);
          ++state;
          return true;
        } else {
          --state;
          return false;
        }
    }
  }

}
