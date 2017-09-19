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
#include <math.h>

#include "common.hpp"
#include "tool.hpp"
#include "interfaces/giindexer.hpp"
#include "interfaces/gfindexer.hpp"

namespace textsim {

class CompressedOptimizedGFIndexer: public GFindexer{
 public:
  //FLAGWIDTH: Depend on the size of Unique freq value
  const static size_t FRONTIERBIT = 19;
  const static size_t FRONTIERVAL = pow(2, 18);
  const static size_t FREQSIZE = 233112;
  const static size_t FREQMAXWIDTH = 38;

  const static unsigned long FLAGNUM = phsim::common::UNISIZE + phsim::common::BISIZE;

  const static unsigned long
      FLAGCAPACITY = CompressedOptimizedGFIndexer::FLAGNUM * CompressedOptimizedGFIndexer::FRONTIERBIT;
  const static unsigned long
      FREQCAPACITY = CompressedOptimizedGFIndexer::FREQSIZE * CompressedOptimizedGFIndexer::FREQMAXWIDTH;


  // Value Rank
  std::bitset<phsim::CompressedOptimizedGFIndexer::FLAGCAPACITY> *flagarray;
  std::bitset<phsim::CompressedOptimizedGFIndexer::FREQCAPACITY> *freqarray;
  // Indexer
  phsim::Indexer *gramindexer;
  // Freq val temp hash
  std::unordered_map<uint64_t, uint32_t> tempfreqhash;
  size_t freqoffset;


 public:
  // index represents logical index similar to C++ arrays.
//  static signed int flagarray_get_range(std::bitset<phsim::CompressedOptimizedGFIndexer::FLAGCAPACITY> *input,
//                                        size_t index) {
//    assert(index <= CompressedOptimizedGFIndexer::FLAGCAPACITY - CompressedOptimizedGFIndexer::FRONTIERBIT);
//    std::bitset<CompressedOptimizedGFIndexer::FLAGCAPACITY>
//        *temp = new std::bitset<phsim::CompressedOptimizedGFIndexer::FLAGCAPACITY>(*input);
//    (*temp) <<= ((CompressedOptimizedGFIndexer::FLAGNUM - index - 1) * CompressedOptimizedGFIndexer::FRONTIERBIT);
//    (*temp) >>= (CompressedOptimizedGFIndexer::FLAGCAPACITY - CompressedOptimizedGFIndexer::FRONTIERBIT);
//    auto val = temp->to_ulong();
//    std::bitset<phsim::CompressedOptimizedGFIndexer::FRONTIERBIT> tempval(val);
//    if (tempval[phsim::CompressedOptimizedGFIndexer::FRONTIERBIT - 1] == 1) {
//      tempval[phsim::CompressedOptimizedGFIndexer::FRONTIERBIT - 1] = 0;
//      return (-(int) tempval.to_ulong());
//    } else {
//      return ((int) val);
//    }
//  };

  static signed int flagarray_get_range(std::bitset<phsim::CompressedOptimizedGFIndexer::FLAGCAPACITY> *input,
                                        size_t index) {
    assert(index < CompressedOptimizedGFIndexer::FLAGNUM);
    std::bitset<phsim::CompressedOptimizedGFIndexer::FRONTIERBIT> temp;
    size_t bitcap = phsim::CompressedOptimizedGFIndexer::FRONTIERBIT;
    while (bitcap > 0) {
      temp.set(bitcap - 1, (*input)[index * CompressedOptimizedGFIndexer::FRONTIERBIT + bitcap - 1]);
      --bitcap;
    }
    if (temp[phsim::CompressedOptimizedGFIndexer::FRONTIERBIT - 1] == 1) {
      temp[phsim::CompressedOptimizedGFIndexer::FRONTIERBIT - 1] = 0;
      return (-(int) temp.to_ulong());
    } else {
      return ((int) temp.to_ulong());
    }
  };

  static void flagarray_set_range(std::bitset<phsim::CompressedOptimizedGFIndexer::FLAGCAPACITY> *flagarray,
                                  size_t index, uint32_t val, bool isoffset) {
    assert(index < CompressedOptimizedGFIndexer::FLAGNUM);
    size_t blockindex = 0;
    while (val > 0) {
      flagarray->set(index * phsim::CompressedOptimizedGFIndexer::FRONTIERBIT + blockindex, val & 1UL);
      ++blockindex;
      val >>= 1;
    }
    if (isoffset) {
      flagarray->set((index + 1) * CompressedOptimizedGFIndexer::FRONTIERBIT - 1, 1);
    }
  };

//  static unsigned long freqarray_get_range(std::bitset<phsim::CompressedOptimizedGFIndexer::FREQCAPACITY> *input,
//                                           size_t index) {
//    assert(index <= CompressedOptimizedGFIndexer::FREQSIZE);
//    std::bitset<CompressedOptimizedGFIndexer::FREQCAPACITY>
//        *temp = new std::bitset<phsim::CompressedOptimizedGFIndexer::FREQCAPACITY>(*input);
//    *(temp) <<= ((CompressedOptimizedGFIndexer::FREQSIZE - index - 1) * CompressedOptimizedGFIndexer::FREQMAXWIDTH);
//    *(temp) >>= ((CompressedOptimizedGFIndexer::FREQCAPACITY - CompressedOptimizedGFIndexer::FREQMAXWIDTH));
//    return temp->to_ulong();
//  };

  static unsigned long freqarray_get_range(std::bitset<phsim::CompressedOptimizedGFIndexer::FREQCAPACITY> *input,
                                           size_t index) {
    assert(index <= CompressedOptimizedGFIndexer::FREQSIZE);
    std::bitset<phsim::CompressedOptimizedGFIndexer::FREQMAXWIDTH> temp;
    size_t bitcap = phsim::CompressedOptimizedGFIndexer::FREQMAXWIDTH;
    while (bitcap > 0) {
      temp.set(bitcap - 1, (*input)[index * CompressedOptimizedGFIndexer::FREQMAXWIDTH + bitcap - 1]);
      --bitcap;
    }
    return temp.to_ulong();
  };

  static void freqarray_set_range(std::bitset<phsim::CompressedOptimizedGFIndexer::FREQCAPACITY> *freqarray,
                                  size_t index,
                                  uint64_t val) {
    assert(index <= CompressedOptimizedGFIndexer::FREQCAPACITY - CompressedOptimizedGFIndexer::FREQMAXWIDTH);
    size_t blockindex = 0;
    while (val > 0) {
      freqarray->set(index * phsim::CompressedOptimizedGFIndexer::FREQMAXWIDTH + blockindex, val & 1UL);
      ++blockindex;
      val >>= 1;
    }
  };

 public:

  CompressedOptimizedGFIndexer(phsim::Indexer *gramindexer) {
    this->gramindexer = gramindexer;
    this->freqoffset = 0;
    this->flagarray = new std::bitset<phsim::CompressedOptimizedGFIndexer::FLAGCAPACITY>();
    this->freqarray = new std::bitset<phsim::CompressedOptimizedGFIndexer::FREQCAPACITY>();
  };

  void inituni(std::string unipath) {
    std::ifstream source;
    source.open(unipath, std::ios::in);
    if (!source.is_open()) {
      throw std::runtime_error("unigrampath not valid");
    }
    while (!source.eof()) {
      std::string current;
      std::getline(source, current);
      if(current.length() < 1){
        continue;
      }
      std::string unigram;
      unsigned long freq;
      phsim::tool::split_string_into_gram_freq(current, unigram, freq);
      id_t id = this->gramindexer->get_unigram_id(unigram);
      if(id >= phsim::CompressedOptimizedGFIndexer::FLAGNUM){
        continue;
      }
      if (freq > phsim::CompressedOptimizedGFIndexer::FRONTIERVAL) {
        if (this->tempfreqhash.find(freq) != this->tempfreqhash.end()) {
          CompressedOptimizedGFIndexer::flagarray_set_range(this->flagarray, id, this->tempfreqhash[freq], 1);
        } else {
          this->tempfreqhash[freq] = this->freqoffset;
          CompressedOptimizedGFIndexer::flagarray_set_range(this->flagarray, id, this->freqoffset, 1);
          CompressedOptimizedGFIndexer::freqarray_set_range(this->freqarray, this->freqoffset, freq);
          ++this->freqoffset;
        }
      } else {
        CompressedOptimizedGFIndexer::flagarray_set_range(this->flagarray, id, freq, 0);
      }
    }
  };

  void initbi(std::string bipath) {
    std::ifstream source;
    source.open(bipath, std::ios::in);
    if (!source.is_open()) {
      throw std::runtime_error("bigrampath not valid");
    }
    while (!source.eof()) {
      std::string current;
      std::getline(source, current);
      if(current.length() < 1){
        continue;
      }
      std::string bigram;
      unsigned long freq;
      phsim::tool::split_string_into_gram_freq(current, bigram, freq);
      id_t id = this->gramindexer->get_bigram_id(bigram);
      if(id >= phsim::CompressedOptimizedGFIndexer::FLAGNUM){
        continue;
      }
      if (freq > phsim::CompressedOptimizedGFIndexer::FRONTIERVAL) {
        if (this->tempfreqhash.find(freq) != this->tempfreqhash.end()) {
          CompressedOptimizedGFIndexer::flagarray_set_range(this->flagarray, id, this->tempfreqhash[freq], 1);
        } else {
          this->tempfreqhash[freq] = this->freqoffset;
          CompressedOptimizedGFIndexer::flagarray_set_range(this->flagarray, id, this->freqoffset, 1);
          CompressedOptimizedGFIndexer::freqarray_set_range(this->freqarray, this->freqoffset, freq);
          ++this->freqoffset;
        }
      } else {
        CompressedOptimizedGFIndexer::flagarray_set_range(this->flagarray, id, freq, 0);
      };
    }
  };

  void clear() {
    this->tempfreqhash.clear();
  }

  ~ CompressedOptimizedGFIndexer() { };

  unsigned long get_freq(std::string query) {
    id_t id;
    if (phsim::tool::gram_length(query)) {
      id = this->gramindexer->get_bigram_id(query);
    } else {
      id = this->gramindexer->get_unigram_id(query);
    }
    if (id == std::numeric_limits<id_t>::max()) {
      return 0;
    }
    int flag = CompressedOptimizedGFIndexer::flagarray_get_range(this->flagarray, id);
    if (flag > 0) {
      return flag;
    } else {
      return CompressedOptimizedGFIndexer::freqarray_get_range(this->freqarray, (size_t) (-flag));
    }
  };
};

};
#endif //TEXTSIM_NAIVEGFINDEXER_HPP
