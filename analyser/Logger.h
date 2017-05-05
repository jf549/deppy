#ifndef LOGGER_H
#define LOGGER_H

#include "types.h"

namespace analyser {

  // Report a loop-dependent dependence.
  void reportDependence(PcT sourcePc, PcT sinkPc, bool sourceIsWrite, bool sinkIsWrite,
                        unsigned int sourceIter, unsigned int sinkIter);

  // Report a loop-independent dependence.
  void reportIndependentDependence(PcT srcPc, PcT sinkPc, bool srcIsWrite, bool sinkIsWrite,
                                   unsigned int iter);

}

#endif // LOGGER_H
