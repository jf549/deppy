#ifndef POINT_H
#define POINT_H

#include <cstdint>

namespace analyser {

  struct Point {
    uint64_t pc;
    unsigned int numAccesses, iterLastAccessed;
    bool isWrite;
  };

}

#endif // POINT_H
