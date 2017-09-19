#ifndef TEXTSIM_NAIVEGFINDEXER_HPP
#define TEXTSIM_NAIVEGFINDEXER_HPP
#include <cmph.h>
#include <string.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <exception>
#include <thread>
#include <stdio.h>
#include <bitset>
#include <unordered_map>
#include <bitset>
#include <sys/timeb.h>
#include <math.h>

#include "common.hpp"
#include "tool.hpp"
#include "interfaces/giindexer.hpp"
#include "interfaces/gfindexer.hpp"


namespace textsim{
class MultiBigram: public GFindexer{
 private:
  unsigned long store[textsim::common::BISIZE];
  textsim::GIIndexer *gramindexer;
  void static buildbigram(std::string chunkpath,textsim::GIIndexer *gramindexer,unsigned long *store){
    std::ifstream source;
    source.open(chunkpath,std::ios::in);
    if(!source.is_open()){
      throw std::runtime_error("bigram chunk not valid");
    }
    while(!source.eof()){
      std::string currentline;
      std::getline(source,currentline);
      std::string gram;
      unsigned long freq;
      if(currentline.length()<2){
        break;
      }

      tool::split_string_into_gram_freq(currentline,gram,freq);
      uint id;
      if(textsim::tool::gram_length(gram)){
        id = gramindexer->get_bigram_id(gram);
      }else{
        id = gramindexer->get_unigram_id(gram);
      }

      if(id >= textsim::common::BISIZE){
        std::cout<<"id larger than BIGRAMSIZE is "<<gram<<" id is "<<id<<std::endl;
        continue;
      }
      store[id] = freq;
    }
  }
 public:
  MultiBigram(textsim::GIIndexer *gramindexer){
    this->gramindexer = gramindexer;
  };

  void initbigram(std::vector<std::string> keysetpathpathinput){
    std::vector<std::thread> threads;
    for(auto i : keysetpathpathinput){
      unsigned long *tempstore = this->store;
      threads.emplace_back(std::thread(MultiBigram::buildbigram,i,this->gramindexer, tempstore));
    }
    for(auto &i : threads){
      i.join();
    }
  }

  unsigned long get_freq(std::string bigram){
    id_t id;
    if(textsim::tool::gram_length(bigram)){
      id = this->gramindexer->get_bigram_id(bigram);
    }else{
      id = this->gramindexer->get_unigram_id(bigram);
    }
    if(id > textsim::common::BISIZE){
      return 0l;
    }
    ulong freq = this->store[id];
    return freq;
  }
};
};
#endif //TEXTSIM_NAIVEGFINDEXER_HPP
