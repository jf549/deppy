#ifndef POINT_H
#define POINT_H

#include <cstdint>

namespace analyser {

  struct Point {
    uint64_t pc;
    bool isWrite;
    unsigned int numAccesses;
    unsigned int iterLastAccessed;
  };

}

#endif // POINT_H
