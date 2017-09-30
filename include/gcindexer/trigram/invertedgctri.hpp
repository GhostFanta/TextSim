#ifndef TEXTSIM_INVERTEDGCTRI_HPP
#define TEXTSIM_INVERTEDGCTRI_HPP

#include "common.hpp"
#include "interfaces/gcindexer.hpp"
#include "interfaces/giindexer.hpp"
#include "interfaces/stopwords.hpp"
#include "tool.hpp"

namespace textsim{
class InvertedGCIndexerTri_x8:public GCIndexer{
 private:
  textsim::GIIndexer *gramindexer;
  textsim::intermediate_x8 *codec;
  textsim::stopwords *stopwordsdic;


  std::vector<uint32_t> **idarray;
  std::vector<uint32_t> **freqarray;
  std::vector<uint32_t> **lengtharray;

  unsigned int frontier;
  uint64_t *escapearray;
  unsigned int escapesize;

 public:
  InvertedGCIndexerTri_x8(textsim::GIIndexer *indexer,
                          textsim::intermediate_x8 *codec,
                          textsim::stopwords *stopwords){
    this->gramindexer = indexer;
    this->codec = codec;
    this->stopwordsdic = stopwords;

    this->idarray = new std::vector<uint32_t> *[textsim::common::UNISIZE * 3];
    this->freqarray = new std::vector<uint32_t> *[textsim::common::UNISIZE * 3];

    for(size_t i = 0 ; i < textsim::common::UNISIZE * 3 ; i++){
      this->idarray[i] = new std::vector<uint32_t>();
      this->freqarray[i] = new std::vector<uint32_t>();
    }
  }

  void initchunks(std::vector<std::string> trigramchunks){
    for(auto i : trigramchunks){
      std::ifstream source;
      source.open();
      if(!source.is_open()){
        throw std::runtime_error("trigram chunk not valid");
      }

      while(!source.eof()){
        std::string current;
        std::getline(source,current);

      };
    }
  }

  void converttodelta(){
    size_t index = 0;
    for(; index < textsim::common::UNISIZE *3 ;++index){
      fastdelta::fast_D1_delta_inplace_x32(this->idarray[index][0]);
    }
  }

  void compress(){
    size_t index = 0;
    std::vector<uint32_t> tempcontextvec;
    std::vector<uint32_t> templengthvec;

    for(; index < textsim::common::UNISIZE * 3 ; ++index){
      tempcontextvec.push_back(this->idarray[0][i]);
      tempcontextvec.push_back(this->freqarray[0][i]);
    };
  }

  void load(){

  }

  void clearcache(){
    for(size_t i =  0 ;i < textsim::common::UNISIZE * 3 ; ++i){
      delete idarray[i];
      delete freqarray[i];
    }
    delete idarray;
    delete freqarray;
  }

  ~InvertedGCIndexerTri_x8(){};

  std::vector<uint64_t> get_contexts(std::string targetphrase){

  }
};
};
#endif //TEXTSIM_INVERTEDGCTRI_HPP
