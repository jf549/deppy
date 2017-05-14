#ifndef DEPENDENCE_RESULTS_H
#define DEPENDENCE_RESULTS_H

#include "types.h"

#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <utility>

namespace analyser {

  using DependenceT = std::tuple<PcT, PcT, bool, bool>;

  void print(DependenceT d);


  class DependenceResults {
  public:
    DependenceResults();
    virtual ~DependenceResults();

    virtual void merge(std::unique_ptr<DependenceResults> other) = 0;

    virtual void addCarriedDependence(PcT srcPc, PcT sinkPc, bool srcIsWrite, bool sinkIsWrite,
                                      unsigned srcIter, unsigned sinkIter) = 0;

    virtual void addIndependentDependence(PcT srcPc, PcT sinkPc, bool srcIsWrite, bool sinkIsWrite,
                                          unsigned iter) = 0;

    virtual void print() = 0;
  };


  class BasicDependenceResults : public DependenceResults {
  public:
    virtual void merge(std::unique_ptr<DependenceResults> other) override;

    virtual void addCarriedDependence(PcT srcPc, PcT sinkPc, bool srcIsWrite, bool sinkIsWrite,
                                      unsigned, unsigned) override;

    virtual void addIndependentDependence(PcT srcPc, PcT sinkPc, bool srcIsWrite, bool sinkIsWrite,
                                          unsigned) override;

    virtual void print() override;

  private:
    std::set<DependenceT> carriedDependences, independentDependences;
  };


  class DetailedDependenceResults : public DependenceResults {
  public:
    virtual void merge(std::unique_ptr<DependenceResults> other) override;

    virtual void addCarriedDependence(PcT srcPc, PcT sinkPc, bool srcIsWrite, bool sinkIsWrite,
                                      unsigned srcIter, unsigned sinkIter) override;

    virtual void addIndependentDependence(PcT srcPc, PcT sinkPc, bool srcIsWrite, bool sinkIsWrite,
                                          unsigned iter) override;

    virtual void print() override;

  private:
    template<typename Val>
    using DependenceTableT = std::multimap<DependenceT, Val>;

    DependenceTableT<std::pair<unsigned, unsigned>> carriedDependences;
    DependenceTableT<unsigned> independentDependences;
  };

}

#endif // DEPENDENCE_RESULTS_H
