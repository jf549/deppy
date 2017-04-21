#include "StrideDetector.h"

#include <algorithm>

namespace analyser {

  StrideDetector::State& operator++(StrideDetector::State& state);
  StrideDetector::State operator++(StrideDetector::State& state, int);
  StrideDetector::State& operator--(StrideDetector::State& state);
  StrideDetector::State operator--(StrideDetector::State& state, int);

  StrideDetector::StrideDetector() : state(State::start) {}

  bool StrideDetector::addAddress(const uint64_t addr) {
    switch (state) {
      case State::start:
        base = addr;
        limit = addr;
        ++state;
        break;

      case State::firstObs:
        base = std::min(base, addr);
        limit = std::max(limit, addr);
        stride = limit - base;
        ++state;
        break;

      case State::strideLnd:
        if (stride == 0) {
          if (addr != base) {
            base = addr;
            limit = addr;
            --state;
          }

        } else if (isInLearnedStride(addr)) {
          base = std::min(base, addr);
          limit = std::max(limit, addr);
          ++state;

        } else {
          base = addr;
          limit = addr;
          --state;
        }
        break;

      case State::weakStride:
      case State::strongStride:
        if (isInLearnedStride(addr)) {
          base = std::min(base, addr);
          limit = std::max(limit, addr);
          ++state;

        } else {
          --state;
          return false;
        }
        break;
    }

    return isAccepting();
  }

  bool StrideDetector::isAccepting() const {
    return state == State::weakStride || state == State::strongStride;
  }

  // An accessed addr is part of the stride iff addr = base + stride * n for some n in N
  // that is, iff (addr - base) / stride = n for some n in N
  bool StrideDetector::isInLearnedStride(const uint64_t addr) const {
    return addr <= limit + stride
           && addr >= base - stride
           && (addr > base ? addr - base : base - addr) % stride == 0;
  }

  StrideDetector::State& operator++(StrideDetector::State& state) {
    switch(state) {
      case StrideDetector::State::start:
        return state = StrideDetector::State::firstObs;

      case StrideDetector::State::firstObs:
        return state = StrideDetector::State::strideLnd;

      case StrideDetector::State::strideLnd:
        return state = StrideDetector::State::weakStride;

      case StrideDetector::State::weakStride:
      case StrideDetector::State::strongStride:
        return state = StrideDetector::State::strongStride;
    }
  }

  StrideDetector::State operator++(StrideDetector::State& state, int) {
    StrideDetector::State tmp(state);
    ++state;
    return tmp;
  }

  StrideDetector::State& operator--(StrideDetector::State& state) {
    switch(state) {
      case StrideDetector::State::start:
      case StrideDetector::State::firstObs:
        return state = StrideDetector::State::start;

      case StrideDetector::State::strideLnd:
        return state = StrideDetector::State::firstObs;

      case StrideDetector::State::weakStride:
        return state = StrideDetector::State::strideLnd;

      case StrideDetector::State::strongStride:
        return state = StrideDetector::State::weakStride;
    }
  }

  StrideDetector::State operator--(StrideDetector::State& state, int) {
    StrideDetector::State tmp(state);
    --state;
    return tmp;
  }

}
