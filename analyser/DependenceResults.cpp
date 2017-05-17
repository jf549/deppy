#include "DependenceResults.h"
#include "DebugInfo.h"

#include <iostream>

namespace analyser {

  void print(DependenceT d) {
    PcT srcPc, sinkPc;
    bool srcIsWrite, sinkIsWrite;
    std::tie(srcPc, sinkPc, srcIsWrite, sinkIsWrite) = d;

    std::cout << (srcIsWrite ? (sinkIsWrite ? "WAW" : "RAW") : "WAR");

    try {
      auto res = getDebugInfo(srcPc);
      std::cout << " (" << res.filename
                << ", line: " << res.lineNum
                << ", col: " << res.colNum;

    } catch (std::out_of_range) {
      std::cout << " (pc: " << srcPc;
    }

    try {
      auto res = getDebugInfo(sinkPc);
      std::cout << ") --> (" << res.filename
                << ", line: " << res.lineNum
                << ", col: " << res.colNum;

    } catch (std::out_of_range) {
      std::cout << ") --> (pc: " << sinkPc;
    }

    std::cout << ")\n";
  }


  DependenceResults::DependenceResults() = default;
  DependenceResults::~DependenceResults() = default;


  void BasicDependenceResults::merge(std::unique_ptr<DependenceResults> other) {
    if (auto ptr = dynamic_cast<BasicDependenceResults*>(other.get())) {
      carriedDependences.insert(ptr->carriedDependences.cbegin(), ptr->carriedDependences.cend());
      independentDependences.insert(ptr->independentDependences.cbegin(),
                                    ptr->independentDependences.cend());
    } else {
      throw std::invalid_argument("other");
    }
  }

  void BasicDependenceResults::addCarriedDependence(PcT srcPc, PcT sinkPc, bool srcIsWrite,
                                                    bool sinkIsWrite, unsigned, unsigned) {
    carriedDependences.emplace(srcPc, sinkPc, srcIsWrite, sinkIsWrite);
  }

  void BasicDependenceResults::addIndependentDependence(PcT srcPc, PcT sinkPc, bool srcIsWrite,
                                                        bool sinkIsWrite, unsigned) {
    independentDependences.emplace(srcPc, sinkPc, srcIsWrite, sinkIsWrite);
  }

  void BasicDependenceResults::print() {
    std::cout << "Loop-carried dependences:\n";

    for (const auto& d : carriedDependences) {
      analyser::print(d);
    }

    std::cout << "\nLoop-independent dependences:\n";

    for (const auto& d : independentDependences) {
      analyser::print(d);
    }
  }


  void DetailedDependenceResults::merge(std::unique_ptr<DependenceResults> other) {
    if (auto ptr = dynamic_cast<DetailedDependenceResults*>(other.get())) {
      carriedDependences.insert(ptr->carriedDependences.cbegin(), ptr->carriedDependences.cend());
      independentDependences.insert(ptr->independentDependences.cbegin(),
                                    ptr->independentDependences.cend());
    } else {
      throw std::invalid_argument("other");
    }
  }

  void DetailedDependenceResults::addCarriedDependence(PcT srcPc, PcT sinkPc, bool srcIsWrite,
    bool sinkIsWrite, unsigned srcIter, unsigned sinkIter) {
    carriedDependences.emplace(DependenceT{ srcPc, sinkPc, srcIsWrite, sinkIsWrite },
                               std::make_pair(srcIter, sinkIter));
  }

  void DetailedDependenceResults::addIndependentDependence(PcT srcPc, PcT sinkPc, bool srcIsWrite,
                                                           bool sinkIsWrite, unsigned iter) {
    independentDependences.emplace(DependenceT{ srcPc, sinkPc, srcIsWrite, sinkIsWrite }, iter);
  }

  void DetailedDependenceResults::print() {
    std::cout << "Loop-carried dependences:\n";

    for (const auto& kv : carriedDependences) {
      analyser::print(kv.first);
      std::cout << "    Iteration: " << kv.second.first << " to " << kv.second.second << '\n';
    }

    std::cout << "\nLoop-independent dependences:\n";

    for (const auto& kv : independentDependences) {
      analyser::print(kv.first);
      std::cout << "    Iteration: " << kv.second << '\n';
    }
  }

}
