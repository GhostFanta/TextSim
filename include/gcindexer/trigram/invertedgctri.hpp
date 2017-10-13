#ifndef TEXTSIM_INVERTEDGCTRI_HPP
#define TEXTSIM_INVERTEDGCTRI_HPP

#include "common.hpp"
#include "interfaces/gcindexer.hpp"
#include "interfaces/giindexer.hpp"
#include "interfaces/stopwords.hpp"
#include "interfaces/intermediate_x8.hpp"
#include "interfaces/intermediate_x32.hpp"
#include "interfaces/intermediate_x64.hpp"
#include "tool.hpp"

namespace textsim{
class Trigramstodigits{
 private:
  struct paircomparator{
    bool operator()(const std::pair<uint64_t ,uint64_t > &a, const std::pair<uint64_t,uint64_t> &b){
      return a.first < b.first;
    }
  };
  std::unordered_map<uint64_t ,std::vector<std::pair<uint64_t,uint64_t>>> temp;

  std::vector<std::string> sourcefilevec;
  std::string targetfilepath;
  textsim::GIIndexer *indexer;

 public:

  Trigramstodigits(std::vector<std::string> sourcefilevec, std::string targetfilepath, textsim::GIIndexer *indexer){
    this->sourcefilevec = sourcefilevec;
    this->targetfilepath = targetfilepath;
    this->indexer = indexer;
  }

  void readandconvert(){
    for(auto i : this->sourcefilevec){
      std::ifstream source;
      source.open(i,std::ios::in);
      if(!source.is_open()){
        throw std::runtime_error("one of trigram chunk is not valid");
      }
      while(!source.eof()){
        std::string current;
        std::getline(source,current);
        if(current.length() < 1){
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
  };

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
          target << j.first << " " << j.second;
          continue;
        }
        target << j.first << " " << j.second<<" ";
        ++index;
      }
      target<<std::endl;
    }
  };
};

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

  uint32_t lengthinfo[textsim::common::UNISIZE];
  std::vector<uint8_t> contextarray[textsim::common::UNISIZE];

 public:
  InvertedGCIndexerTri_x8(textsim::GIIndexer *indexer,
                          textsim::intermediate_x8 *codec,
                          textsim::stopwords *stopwords){
    this->gramindexer = indexer;
    this->codec = codec;
    this->stopwordsdic = stopwords;

    this->idarray = new std::vector<uint32_t> *[textsim::common::UNISIZE];
    this->freqarray = new std::vector<uint32_t> *[textsim::common::UNISIZE];

    for(size_t i = 0 ; i < textsim::common::UNISIZE ; i++){
      this->idarray[i] = new std::vector<uint32_t>();
      this->freqarray[i] = new std::vector<uint32_t>();
    }
  }

  void initchunks(std::vector<std::string> trigramchunks){
    for(auto i : trigramchunks){
      std::ifstream source;
      source.open(i,std::ios::in);

      if(!source.is_open()){
        throw std::runtime_error("trigram chunk not valid");
      }

      while(!source.eof()){
        std::string current;
        std::getline(source,current);
        if(current.length() < 1){
          break;
        }
        std::vector<std::string> temp = textsim::tool::string_split(current,' ');
        std::string targetphrase = temp[0];
        for(size_t i = 1 ; i < temp.size() ; i += 2){
          this->idarray[i]->push_back((uint32_t)std::stoul(temp[i]));
          this->freqarray[i]->push_back((uint32_t)std::stoul(temp[i+1]));
        };
      };
    }
  }

  void converttodelta(){
    size_t index = 0;
    for(; index < textsim::common::UNISIZE ;++index){
      fastdelta::fast_D1_delta_inplace_x32(this->idarray[index][0]);
    }
  }

  void compress(){
    size_t index = 0;
    std::vector<uint32_t> temp;

    for(; index < textsim::common::UNISIZE; ++index){
      for(size_t j = 0 ; j < this->idarray[index]->size() ; ++j){
        temp.push_back(this->idarray[index][0][j]);
        temp.push_back(this->freqarray[index][0][j]);
      };
      this->lengthinfo[index] = this->idarray[index]->size();
      size_t tempsize = temp.size();
      codec->encode_x8(temp,tempsize,this->contextarray[index],tempsize);
    };
  }

  void seralize(){

  };

  void load(){

  };

  void clearcache(){
    for(size_t i =  0 ;i < textsim::common::UNISIZE; ++i){
      delete idarray[i];
      delete freqarray[i];
    }
    delete idarray;
    delete freqarray;
  }

  ~InvertedGCIndexerTri_x8(){};

  void get_contexts(std::string targetphrase,
                       std::vector<std::pair<uint64_t,uint64_t>> &contextarray){
    size_t targetphraseid = this->gramindexer->get_unigram_id(targetphrase);
    auto compressedcontextarray = this->contextarray[targetphraseid];
    size_t contextarraylength = this->lengthinfo[targetphraseid] * 2;
    size_t contextarraylengthcopy = contextarraylength;
//    this->codec->decode_x8(compressedcontextarray,contextarraylength,contextarray,contextarraylengthcopy);
  }
};

class InvertedGCIndexerTri_x32:public GCIndexer{
 private:
  textsim::GIIndexer *gramindexer;
  textsim::intermediate_x32 *codec;
  textsim::stopwords *stopwordsdic;

  std::vector<uint32_t> **idarray;
  std::vector<uint32_t> **freqarray;
  std::vector<uint32_t> **lengtharray;

  unsigned int frontier;
  uint64_t *escapearray;
  unsigned int escapesize;

  std::vector<uint32_t> lengthinfo[textsim::common::UNISIZE];
  std::vector<uint32_t> contextarray[textsim::common::UNISIZE];

 public:
  InvertedGCIndexerTri_x32(textsim::GIIndexer *indexer,
                            textsim::intermediate_x32 *codec,
                          textsim::stopwords *stopwords){

  }

  void initchunks(){

  }

  void converttodelta(){

  }

  void compress(){

  }

  void load(){

  }

  void clearcache(){

  }

  ~InvertedGCIndexerTri_x32(){

  };

  void get_contexts(std::string targetphrase,
                    std::vector<std::pair<uint64_t,uint64_t>> &contextarray){

  }
};

class InvertedGCIndexerTri_x64:public GCIndexer{
  textsim::GIIndexer *gramindexer;
  textsim::intermediate_x64 *codec;
  textsim::stopwords *stopwordsdic;

  std::vector<uint32_t> **idarray;
  std::vector<uint32_t> **freqarray;
  std::vector<uint32_t> **lengtharray;

  unsigned int frontier;
  uint64_t *escapearray;
  unsigned int escapesize;

  std::vector<uint32_t> lengthinfo[textsim::common::UNISIZE];
  std::vector<uint64_t> contextarray[textsim::common::UNISIZE];
};
};
#endif //TEXTSIM_INVERTEDGCTRI_HPP
