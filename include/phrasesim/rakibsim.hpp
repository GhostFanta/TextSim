#ifndef TEXTSIM_RAKIBSIM_HPP
#define TEXTSIM_RAKIBSIM_HPP

#include "common.hpp"
#include "tool.hpp"
#include "interfaces/phrasesim.hpp"
#include "intersection/galloping.hpp"
#include "interfaces/intersection.hpp"
#include "interfaces/giindexer.hpp"
#include "interfaces/gfindexer.hpp"
#include "interfaces/gcindexer.hpp"

namespace textsim{
class rakibsim : public Phrasesim{
 private:
  textsim::intersection *intersectionmethod;
  textsim::GIIndexer *giindexer;
  textsim::GFindexer *gfindexer;
  textsim::GCIndexer *gcindexer;

 public:
  rakibsim(textsim::GIIndexer *gramindexer,
           textsim::GFindexer *gfindexer,
           textsim::GCIndexer *gcIndexer,
           textsim::intersection *intersectionmethod){
    this->giindexer = gramindexer;
    this->gfindexer = gfindexer;
    this->gcindexer = gcindexer;
    this->intersectionmethod = intersectionmethod;
  }

  double phrasesimilarity(std::string gram1,std::string gram2){

    auto contextarray1 = this->gcindexer->get_contexts();

    std::vector<std::pair<uint64_t,uint64_t>> overlapped1;
    std::vector<std::pair<uint64_t,uint64_t>> overlapped2;
  };

 private:
  void context_intersection(std::vector<std::pair<uint64_t,uint64_t>> &contextarray1,
                            std::vector<std::pair<uint64_t,uint64_t>> &contextarray2,
                            std::vector<std::pair<uint64_t,uint64_t>> &overlapped1,
                            std::vector<std::pair<uint64_t,uint64_t>> &overlapped2){
    this->intersectionmethod->intersect(contextarray1,contextarray2,overlapped1,overlapped2);

  }

  void statisticalpruning(){

  }

  double strength(){

  }


  double rsob(){

  };

  double cossim(){

  }

  double relatedness(){

  }

  double ngd(std::string gram1,std::string gram2,double relatednessval){
    double res = 0;
  }
};
}
#endif //TEXTSIM_RAKIBSIM_HPP