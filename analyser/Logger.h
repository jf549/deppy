#ifndef LOGGER_H
#define LOGGER_H

#include <cstdint>

namespace analyser {

  // Report a loop-dependent dependence.
  void reportDependence(uint64_t sourcePc, uint64_t sinkPc, bool sourceIsWrite, bool sinkIsWrite,
                        unsigned int sourceIter, unsigned int sinkIter);

  // Report a loop-independent dependence.
  void reportIndependentDependence(uint64_t srcPc, uint64_t sinkPc, bool srcIsWrite,
                                   bool sinkIsWrite, unsigned int iter);

}

#endif // LOGGER_H
