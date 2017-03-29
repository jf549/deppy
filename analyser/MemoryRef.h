#ifndef MEMORY_REF_H
#define MEMORY_REF_H

#include <cstdint>
#include <memory>

namespace analyser {

  struct Point {
    uint64_t pc;
    bool isWrite;
    unsigned int numAccesses;
    unsigned int iterLastAccessed;
    std::unique_ptr<Point> next;
  };

  struct Stride {
    uint64_t base, stride, limit;
    unsigned int numAccesses;
  };

}

#endif // MEMORY_REF_H
