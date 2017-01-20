#include <stdint.h>

namespace analyser {
  struct MemoryAccess {
    MemoryAccess *next;
  };

  struct Point : public MemoryAccess {
    uint64_t addr;
    int numAccesses;
    bool isWrite;
    int lastIteration;
  };

  struct Stride : public MemoryAccess {
    uint64_t base, limit, dist;
    int numAccesses;
  };
}
