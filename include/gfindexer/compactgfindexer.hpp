#ifndef TEXTSIM_COMPACTGFINDEXER_HPP
#define TEXTSIM_COMPACTGFINDEXER_HPP
#include <bitset>
#include <math.h>

#include "interfaces/giindexer.hpp"
#include "interfaces/gfindexer.hpp"
#include "common.hpp"
#include "tool.hpp"

namespace textsim {

class CompressedOptimizedGFIndexer: public GFindexer{
 public:
  //FLAGWIDTH: Depend on the size of Unique freq value
  const static size_t FRONTIERVAL = pow(2, 18);
  const static size_t FREQSIZE = 233112;
  const static size_t FREQMAXWIDTH = 38;

  const static unsigned long FLAGNUM = textsim::common::UNISIZE + textsim::common::BISIZE;

  const static unsigned long
      FLAGCAPACITY = CompressedOptimizedGFIndexer::FLAGNUM * textsim::common::FRONTIERBIT;
  const static unsigned long
      FREQCAPACITY = CompressedOptimizedGFIndexer::FREQSIZE * textsim::common::FREQMAXWIDTH;


  // Value Rank
  std::bitset<textsim::CompressedOptimizedGFIndexer::FLAGCAPACITY> *flagarray;
  std::bitset<textsim::CompressedOptimizedGFIndexer::FREQCAPACITY> *freqarray;
  // Indexer
  textsim::GIIndexer *gramindexer;
  // Freq val temp hash
  std::unordered_map<uint64_t, uint32_t> tempfreqhash;
  size_t freqoffset;


 public:

  static signed int flagarray_get_range(std::bitset<textsim::CompressedOptimizedGFIndexer::FLAGCAPACITY> *input,
                                        size_t index) {
    assert(index < CompressedOptimizedGFIndexer::FLAGNUM);
    std::bitset<textsim::common::FRONTIERBIT> temp;
    size_t bitcap = textsim::common::FRONTIERBIT;
    while (bitcap > 0) {
      temp.set(bitcap - 1, (*input)[index * textsim::common::FRONTIERBIT + bitcap - 1]);
      --bitcap;
    }
    if (temp[textsim::common::FRONTIERBIT - 1] == 1) {
      temp[textsim::common::FRONTIERBIT - 1] = 0;
      return (-(int) temp.to_ulong());
    } else {
      return ((int) temp.to_ulong());
    }
  };

  static void flagarray_set_range(std::bitset<textsim::CompressedOptimizedGFIndexer::FLAGCAPACITY> *flagarray,
                                  size_t index, uint32_t val, bool isoffset) {
    assert(index < CompressedOptimizedGFIndexer::FLAGNUM);
    size_t blockindex = 0;
    while (val > 0) {
      flagarray->set(index * textsim::common::FRONTIERBIT + blockindex, val & 1UL);
      ++blockindex;
      val >>= 1;
    }
    if (isoffset) {
      flagarray->set((index + 1) * textsim::common::FRONTIERBIT - 1, 1);
    }
  };


  static unsigned long freqarray_get_range(std::bitset<textsim::CompressedOptimizedGFIndexer::FREQCAPACITY> *input,
                                           size_t index) {
    assert(index <= CompressedOptimizedGFIndexer::FREQSIZE);
    std::bitset<textsim::CompressedOptimizedGFIndexer::FREQMAXWIDTH> temp;
    size_t bitcap = textsim::CompressedOptimizedGFIndexer::FREQMAXWIDTH;
    while (bitcap > 0) {
      temp.set(bitcap - 1, (*input)[index * CompressedOptimizedGFIndexer::FREQMAXWIDTH + bitcap - 1]);
      --bitcap;
    }
    return temp.to_ulong();
  };

  static void freqarray_set_range(std::bitset<textsim::CompressedOptimizedGFIndexer::FREQCAPACITY> *freqarray,
                                  size_t index,
                                  uint64_t val) {
    assert(index <= CompressedOptimizedGFIndexer::FREQCAPACITY - CompressedOptimizedGFIndexer::FREQMAXWIDTH);
    size_t blockindex = 0;
    while (val > 0) {
      freqarray->set(index * textsim::CompressedOptimizedGFIndexer::FREQMAXWIDTH + blockindex, val & 1UL);
      ++blockindex;
      val >>= 1;
    }
  };

 public:

  CompressedOptimizedGFIndexer(textsim::GIIndexer *gramindexer) {
    this->gramindexer = gramindexer;
    this->freqoffset = 0;
    this->flagarray = new std::bitset<textsim::CompressedOptimizedGFIndexer::FLAGCAPACITY>();
    this->freqarray = new std::bitset<textsim::CompressedOptimizedGFIndexer::FREQCAPACITY>();
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
      textsim::tool::split_string_into_gram_freq(current, unigram, freq);
      id_t id = this->gramindexer->get_unigram_id(unigram);
      if(id >= textsim::CompressedOptimizedGFIndexer::FLAGNUM){
        continue;
      }
      if (freq > textsim::CompressedOptimizedGFIndexer::FRONTIERVAL) {
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
      textsim::tool::split_string_into_gram_freq(current, bigram, freq);
      id_t id = this->gramindexer->get_bigram_id(bigram);
      if(id >= textsim::CompressedOptimizedGFIndexer::FLAGNUM){
        continue;
      }
      if (freq > textsim::CompressedOptimizedGFIndexer::FRONTIERVAL) {
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
    if (textsim::tool::gram_length(query)) {
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
}
#endif //TEXTSIM_COMPACTGFINDEXER_HPP
