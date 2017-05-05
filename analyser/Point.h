#ifndef POINT_H
#define POINT_H

#include "types.h"

namespace analyser {

  struct Point {
    PcT pc;
    unsigned int iterLastAccessed;
    bool isWrite;
  };

}

#endif // POINT_H
