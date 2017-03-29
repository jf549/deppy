#ifndef STRIDE_LOOP_H
#define STRIDE_LOOP_H

#include "Loop.h"
#include "MemoryRef.h"
#include "StrideDetector.h"

#include <map>

namespace analyser {

  class StrideLoop : public Loop {

  private:
    std::multimap<uint64_t /* pc */, Stride> pendingStrideTable, historyStrideTable;
    std::map<uint64_t /* pc */, StrideDetector> detectors;
  };

}

#endif // STRIDE_LOOP_H
