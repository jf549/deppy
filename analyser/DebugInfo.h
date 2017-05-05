#ifndef DEBUG_INFO_H
#define DEBUG_INFO_H

#include "types.h"

#include <string>

namespace analyser {

  struct DebugInfo {
    std::string filename;
    int lineNum, colNum;
  };

  DebugInfo getDebugInfo(const PcT pc);

}

#endif // DEBUG_INFO_H
