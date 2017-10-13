#ifndef TEXTSIM_INVERTEDGCFOUR_HPP
#define TEXTSIM_INVERTEDGCFOUR_HPP

#include "common.hpp"
#include "tool.hpp"
#include "interfaces/giindexer.hpp"
#include "interfaces/gcindexer.hpp"
#include "interfaces/stopwords.hpp"
#include "interfaces/intermediate_x64.hpp"
#include "interfaces/intermediate_x32.hpp"
#include "interfaces/intermediate_x8.hpp"

namespace textsim {

class Fourgramtodigits{
 private:
  struct paircomparator{
    bool operator()(const std::pair<uint64_t ,uint64_t > &a, const std::pair<uint64_t,uint64_t> &b){
      return a.first < b.first;
    }
  };

  std::unordered_map<uint64_t, std::vector<std::pair<uint64_t ,uint64_t>>> temp;

  std::vector<std::string> sourcefilevec;
  std::string targtefilepath;
  textsim::GIIndexer *indexer;

 public:
  Fourgramtodigits(std::vector<std::string> sourcefilevec, std::string targetfilepath,textsim::GIIndexer *indexer){
    this->sourcefilevec = sourcefilevec;
    this->targtefilepath = targetfilepath;
    this->indexer = indexer;
  };

  void readandconver(){
    for(auto i : this->sourcefilevec){
      std::ifstream source;
      source.open(i,std::ios::in);
      if(!source.is_open()){
        throw std::runtime_error("one of trigram chunk is not valid");
      }
      while(!source.eof()){
        std::string current;
        std::getline(source,current);
        if(current.length()<1){
          break;
        }
        std::vector<std::string> comps = textsim::tool::string_split(current,' ');
        std::string targetphrase = comps[0];
        uint64_t targetphraseid = this->indexer->get_unigram_id(targetphrase);


        for(size_t i = 4 ; i < comps.size(); i+=3){
          std::string context = comps[i-2]+" "+comps[i-1];
          //   std::cout << "getting context id for " << context << "\t" << std::endl;
          uint64_t contextid = this->indexer->get_bigram_id(context);
          if(contextid == std::numeric_limits<uint32_t>::max()){
            continue;
          }
          uint64_t contextfreq = std::stoul(comps[i]);
          std::pair<uint64_t ,uint64_t > temp = {contextid,contextfreq};
          this->temp[targetphraseid].push_back(temp);
        }
      }
    }
  }

  void sort(){
    for(auto i : this->temp){
      std::sort(i.second.begin(),i.second.end(),paircomparator());
    }
  }

  void output(){
    std::ofstream target;
    target.open(this->targetfilepath,std::ios::out);
    if(!target.is_open()){
      throw std::runtime_error("output path is not valid");
    }

    for(auto i : this->temp){
      target<<i.first<<"\t";
      size_t index = 0;
      for(auto j : i.second){
        if(index == i.second.size() - 1){
          target<<j.first<<" "<<j.second<<" ";
          continue;
        }
        target<<j.first<<" "<<j.second<<" ";
        ++index;
      }
      target<<std::endl;
    }
  };
};


class InvertedGCIndexerFour_x8:public GCIndexer{
 private:
  textsim::GIIndexer *gramindexer;
  textsim::intermediate_x8 *codec;
  textsim::stopwords *stopwordsdic;

  std::vector<uint32_t> **idarray;
  std::vector<uint32_t> **freqarray;
  std::vector<uint32_t> **lengtharray;

  unsigned  int frontier;
  uint64_t *escapearray;
  unsigned int escapearray;

  std::vector<uint32_t > lengthinfo[textsim::common::BISIZE];
  std::vector<uint8_t> contextarray[textsim::common::BISIZE];

 public:

  InvertedGCIndexerFour_x8(textsim::GIIndexer *gramindexer,
                            textsim::intermediate_x8 *codec,
                            textsim::stopwords *stopwords){

  }

  void get_contexts(std::string targetphrase,
                    std::vector<std::pair<uint64_t,uint64_t>> &contextarray){

  }
};

class InvertedGCIndexerFour_x8:public GCIndexer{
 private:

 public:
  void get_contexts(std::string targetphrase,
                    std::vector<std::pair<uint64_t,uint64_t>> &contextarray){

  }
};

class InvertedGCIndexerFour_x8:public GCIndexer{
 private:

 public:
  void get_contexts(std::string targetphrase,
                    std::vector<std::pair<uint64_t,uint64_t>> &contextarray){

  }
};
};
#endif //TEXTSIM_INVERTEDGCFOUR_HPP
