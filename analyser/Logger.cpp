#include "Logger.h"
#include "DebugInfo.h"

#include <iostream>

namespace analyser {

#ifdef EXTBENCH

void reportDependence(PcT, PcT, bool, bool, unsigned int, unsigned int) {}

void reportIndependentDependence(PcT, PcT, bool, bool, unsigned int) {}

#else

  void reportDependence(PcT srcPc, PcT sinkPc, bool srcIsWrite, bool sinkIsWrite,
                        unsigned int srcIter, unsigned int sinkIter) {
    std::cout << "Loop-carried "
              << (srcIsWrite ? (sinkIsWrite ? "WAW" : "RAW") : "WAR");

    try {
      auto res = getDebugInfo(srcPc);
      std::cout << " (file: " << res.filename
                << ", line: " << res.lineNum
                << ", col: " << res.colNum;

    } catch (std::out_of_range) {
      std::cout << " (line: " << srcPc;
    }

    std::cout << ", iteration: " << srcIter;

    try {
      auto res = getDebugInfo(srcPc);
      std::cout << ") --> (file: " << res.filename
                << ", line: " << res.lineNum
                << ", col: " << res.colNum;

    } catch (std::out_of_range) {
      std::cout << ") --> (line: " << sinkPc;
    }

    std::cout << ", iteration: " << sinkIter << ")\n";
  }

  void reportIndependentDependence(PcT srcPc, PcT sinkPc, bool srcIsWrite,
                                   bool sinkIsWrite, unsigned int iter) {
    std::cout << "Loop-independent "
              << (srcIsWrite ? (sinkIsWrite ? "WAW" : "RAW") : "WAR");

    try {
      auto res = getDebugInfo(srcPc);
      std::cout << " (file: " << res.filename
                << ", line: " << res.lineNum
                << ", col: " << res.colNum;

    } catch (std::out_of_range) {
      std::cout << " (line: " << srcPc;
    }

    try {
      auto res = getDebugInfo(srcPc);
      std::cout << ") --> (file: " << res.filename
                << ", line: " << res.lineNum
                << ", col: " << res.colNum;

    } catch (std::out_of_range) {
      std::cout << ") --> (line: " << sinkPc;
    }

    std::cout << ") iteration: " << iter << '\n';
  }

#endif

}
