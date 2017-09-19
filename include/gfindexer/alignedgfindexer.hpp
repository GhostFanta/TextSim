#ifndef TEXTSIM_ALIGNEDGFINDEXER_HPP
#define TEXTSIM_ALIGNEDGFINDEXER_HPP

#include <bitset>
#include <cstring>
#include <math.h>
#include "common.hpp"
#include "tool.hpp"
#include "interfaces/giindexer.hpp"
#include "interfaces/gfindexer.hpp"

namespace textsim {

class CompressedmixGFIndexer : public GFindexer {

 public:
  textsim::Indexer *gramindexer;

  unsigned long frontier;
  unsigned long escapeoffset = 0;

  unsigned int mainarray[textsim::common::UNISIZE + textsim::common::BISIZE];
  unsigned long escapearray[textsim::common::ESCAPEARRAYSIZE];

 public:
  // Constructors and interfaces
  CompressedmixGFIndexer(textsim::Indexer *gramindexer) {
    this->gramindexer = gramindexer;
    this->escapeoffset = 0;
    this->frontier = pow(2, CompressedmixGFIndexer::SMALLVALUEWIDTH); //Freq larger than this one will be large freq;
  }

  void inituni(std::vector<std::string> unigram_freq_chunks) {
    for (std::string file : unigram_freq_chunks) {
      std::ifstream source;
      source.open(file, std::ios::in);
      if (!source.is_open()) {
        throw std::runtime_error("uni chunk not valid");
      }
      unsigned long index = 0;

      unsigned long getlineinterval = 0;
      unsigned long splitinterval = 0;
      unsigned long getidinterval = 0;
      unsigned long insertinterval = 0;

      while (!source.eof()) {
        ++index;
        auto t1 = std::chrono::high_resolution_clock::now();
        std::string current;
        std::getline(source, current);
        unsigned long freq;
        if (current.length() < 2) {
          continue;
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        std::string gramcontent;
        textsim::tool::split_string_into_gram_freq(current, gramcontent, freq);
        auto t3 = std::chrono::high_resolution_clock::now();
        auto flagoffset = this->gramindexer->get_unigram_id(gramcontent);
        if (flagoffset > CompressedmixGFIndexer::UNISZIE + CompressedmixGFIndexer::BISIZE) {
          continue;
        }
        auto t4 = std::chrono::high_resolution_clock::now();
        unsigned int flag;
        if (freq >= this->frontier) {
          // std::cout<<"freq is larger than 2^32 "<<current<<std::endl;
          flag = 2147483648;
          flag |= this->escapeoffset++;
          this->escapearray[this->escapeoffset] = freq;
          this->mainarray[flagoffset] = flag;
          // std::cout<<"escape set insertion: "<<current<<"\t\tescapeoffset "<<this->escapeoffset<<std::endl;
          // std::cout<<"flag is "<<flag<<std::endl;
        } else {
          flag = 0;
          flag |= freq;
          this->mainarray[flagoffset] = flag;
        }
        auto t5 = std::chrono::high_resolution_clock::now();

        getlineinterval += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        splitinterval += std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
        getidinterval += std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
        insertinterval += std::chrono::duration_cast<std::chrono::microseconds>(t5 - t4).count();
        if (index % 1000000 == 0) {
          // std::cout<<"getlineinterval: "<<getlineinterval<<std::endl;
          // std::cout<<"splitinterval: "<<splitinterval<<std::endl;
          // std::cout<<"getidinterval: "<<getidinterval<<std::endl;
          // std::cout<<"insertinterval: "<<insertinterval<<std::endl;
          getlineinterval = 0;
          splitinterval = 0;
          getidinterval = 0;
          insertinterval = 0;
        }
      }
    }
  }

  void initbi(std::vector<std::string> bigram_freq_chunks) {
    for (std::string file : bigram_freq_chunks) {
      std::ifstream source;
      source.open(file, std::ios::in);
      if (!source.is_open()) {
        throw std::runtime_error("bichunk not valid");
      }

      unsigned long index = 0;

      unsigned long getlineinterval = 0;
      unsigned long splitinterval = 0;
      unsigned long getidinterval = 0;
      unsigned long insertinterval = 0;

      while (!source.eof()) {
        ++index;
        auto t1 = std::chrono::high_resolution_clock::now();
        std::string current;
        std::getline(source, current);
        std::string gramcontent;
        unsigned long freq;
        if (current.length() < 2) {
          continue;
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        textsim::tool::split_string_into_gram_freq(current, gramcontent, freq);
        auto t3 = std::chrono::high_resolution_clock::now();
        auto flagoffset = this->gramindexer->get_bigram_id(gramcontent);
        if (flagoffset > CompressedmixGFIndexer::UNISZIE + CompressedmixGFIndexer::BISIZE) {
          continue;
        }
        auto t4 = std::chrono::high_resolution_clock::now();
        unsigned int flag;
        if (freq > this->frontier) {
          // std::cout<<"freq is larger than 2^32 "<<current<<std::endl;
          flag = 2147483648;
          flag |= this->escapeoffset++;
          this->escapearray[this->escapeoffset] = freq;
          this->mainarray[flagoffset] = flag;
          // std::cout<<"escape set insertion: "<<current<<"\t\tescapeoffset "<<this->escapeoffset<<std::endl;
          // std::cout<<"flag is "<<flag<<std::endl;
        } else {
          flag = 0;
          flag |= freq;
          this->mainarray[flagoffset] = flag;
        }
        auto t5 = std::chrono::high_resolution_clock::now();

        getlineinterval += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        splitinterval += std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
        getidinterval += std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
        insertinterval += std::chrono::duration_cast<std::chrono::microseconds>(t5 - t4).count();
        if (index % 1000000 == 0) {
          // std::cout<<"getlineinterval: "<<getlineinterval<<std::endl;
          // std::cout<<"splitinterval: "<<splitinterval<<std::endl;
          // std::cout<<"getidinterval: "<<getidinterval<<std::endl;
          // std::cout<<"insertinterval: "<<insertinterval<<std::endl;
          getlineinterval = 0;
          splitinterval = 0;
          getidinterval = 0;
          insertinterval = 0;
        }
      }
    }
  }

  ~CompressedmixGFIndexer() {}

  unsigned long get_freq(std::string input) {
    id_t offset;

    if (textsim::tool::gram_length(input)) {
      offset = this->gramindexer->get_bigram_id(input);
    } else {
      offset = this->gramindexer->get_unigram_id(input);
    }
    if (offset > CompressedmixGFIndexer::UNISZIE + CompressedmixGFIndexer::BISIZE) {
      return 0;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    unsigned int flag = this->mainarray[offset];
    auto t2 = std::chrono::high_resolution_clock::now();
    auto count = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    std::cout << "t2 - t1\t" << count << std::endl;

    if ((int) flag > 0) {
      return flag;
    } else {
      auto t3 = std::chrono::high_resolution_clock::now();
      flag <<= 1;
      flag >>= 1;
      std::bitset<32> flagset(flag);
      auto t4 = std::chrono::high_resolution_clock::now();
      auto count1 = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
      std::cout << "t4 - t3\t" << count1 << std::endl;
      return this->escapearray[flag];
    }
  }
};
}
#endif //TEXTSIM_ALIGNEDGFINDEXER_HPP
