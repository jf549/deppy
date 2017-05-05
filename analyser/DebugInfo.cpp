#include "DebugInfo.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

namespace analyser {

  DebugInfo getDebugInfo(const PcT pc) {
    static std::unordered_map<PcT, DebugInfo> infoMap;
    static bool init = false;

    if (!init) {
      std::ifstream debugInfo("debug_info");
      std::string line;

      while (std::getline(debugInfo, line)) {
        DebugInfo di;
        PcT pc_;
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
