#include "DebugInfo.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

namespace analyser {

  DebugInfo getDebugInfo(const uint64_t pc) {
    static std::unordered_map<uint64_t, DebugInfo> infoMap;
    static bool init = false;

    if (!init) {
      std::ifstream debugInfo("debug_info");
      std::string line;

      while (std::getline(debugInfo, line)) {
        DebugInfo di;
        uint64_t pc_;
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream lineStream(line);
        lineStream >> pc_ >> di.filename >> di.lineNum >> di.colNum;
        infoMap.emplace(pc_, di);
      }

      init = true;
    }

    return infoMap.at(pc);
  }

}
