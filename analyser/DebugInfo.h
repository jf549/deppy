#ifndef DEBUG_INFO_H
#define DEBUG_INFO_H

#include <cstdint>
#include <string>

namespace analyser {

  struct DebugInfo {
    std::string filename;
    int lineNum, colNum;
  };

  DebugInfo getDebugInfo(const uint64_t pc);

}

#endif // DEBUG_INFO_H
