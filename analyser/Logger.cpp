#include "Logger.h"
#include "DebugInfo.h"

#include <iostream>

namespace analyser {

  void reportDependence(uint64_t srcPc, uint64_t sinkPc, bool srcIsWrite, bool sinkIsWrite,
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

  void reportIndependentDependence(uint64_t srcPc, uint64_t sinkPc, bool srcIsWrite,
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

}
