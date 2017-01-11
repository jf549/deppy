#include <stdint.h>

namespace analyser {
  enum class State : int {start, firstObs, strideLnd, weakStride, strongStride};

  struct Stride {
    uint64_t base, diff, limit;
  };

  class StrideDetector {
  public:
    StrideDetector(int pc, bool isWrite, uint64_t addr);

    Stride getStride() {return stride;}
    int getPc() {return pc;}
    bool getIsWrite() { return isWrite; }

    void addAddress(uint64_t addr);

  private:
    State s;
    Stride stride;
    int pc;
    bool isWrite;
  };
}
