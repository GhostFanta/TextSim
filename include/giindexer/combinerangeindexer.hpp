#ifndef TEXTSIM_COMBINERANGEINDEXER_HPP
#define TEXTSIM_COMBINERANGEINDEXER_HPP

#include "common.hpp"
#include "tool.hpp"

namespace textsim {
class CombineRangeIndexer : public GIIndexer {
 private:
  bool initflag;
 public:
  const static unsigned int IDWIDTH = 23;

 private:
  std::string uniarray[textsim::common::UNISIZE];

  std::bitset<textsim::common::IDWIDTH * 2> BIARRAY[textsim::common::BISIZE];
  std::bitset<textsim::common::IDWIDTH * 2> CONARRAY[textsim::common::CONSIZE];

  cmph_t *hashuni;
  cmph_t *hashbi;
  cmph_t *hashcon;

  FILE *mphfileuni;
  FILE *mphfilebi;
  FILE *mphfilecon;

 public:
  //Test comps
  std::unordered_map<std::string, uint_fast32_t> bigramtempcheck;
  std::unordered_map<std::string, uint_fast32_t> contexttempcheck;

 public:
  void static initonechunk(std::string onechunkpath, cmph_t *hashuni, std::string *uniarray) {
    char *line;
    size_t len = 0;
    ssize_t read;
    const char *keysetpath = &onechunkpath[0u];
    FILE *keyset = fopen(keysetpath, "r");

    if (keyset == NULL) {
      throw std::runtime_error("this key set chunk is not valid");
    }
    if (hashuni == NULL) {
      throw std::runtime_error("the hash function is not ready");
    }
    while ((read = getline(&line, &len, keyset)) != -1) {
      line[strlen(line) - 1] = '\0';
      unsigned int id = cmph_search(hashuni, line, (cmph_uint32) strlen(line));
      uniarray[id] = std::string(line);
    }
  };

 public:

  CombineRangeIndexer(std::string unipathinput, std::string biflaginputpath, std::string conflaginputpath) {

  }

  CombineRangeIndexer(std::string unimphpathinput) {
    this->initflag = false;
    const char *mphpathuni = &unimphpathinput[0u];
    mphfileuni = fopen(mphpathuni, "r");
    this->hashuni = cmph_load(mphfileuni);
    this->hashbi = NULL;
  }

  CombineRangeIndexer(std::string unimphpath, std::string bimphpath) {
    this->initflag = false;
    const char *mphpathuni = &unimphpath[0u];
    mphfileuni = fopen(mphpathuni, "r");
    this->hashuni = cmph_load(mphfileuni);
  }

  void initcmphbi(std::string bimphpath) {
    const char *mphpathbi = &bimphpath[0u];
    mphfilebi = fopen(mphpathbi, "r");
    this->hashbi = cmph_load(mphfilebi);
  }

  void initcmphcon(std::string conmphpath) {
    const char *mphpathcon = &conmphpath[0u];
    mphfilecon = fopen(mphpathcon, "r");
    this->hashcon = cmph_load(mphfilecon);
  }

  void init(std::vector<std::string> unichunks) {
    std::vector<std::thread> threads;
    for (auto i : unichunks) {
      threads.emplace_back(std::thread(textsim::CombineRangeIndexer::initonechunk, i, this->hashuni, this->uniarray));
    }
    for (auto &i : threads) {
      i.join();
    }
  }

  void initbi(std::string bigramset) {
    std::ifstream source;
    source.open(bigramset, std::ios::in);

    if (!source.is_open()) {
      throw std::runtime_error("bigram dataset not valid");
    }

    std::ofstream check;
    check.open("/raid6/workspace/zichu/phsim/PhSim/build/src/bicheck", std::ios::out);
    if (!check.is_open()) {
      throw std::runtime_error("bichek not open");
    }

    check << "ngram\t\tinside bitarray\t\tdigitkey\t\tpos" << std::endl;

    long index = 0;
    while (!source.eof()) {
      std::string current;
      std::getline(source, current);
      std::cout << current << std::endl;
      std::string unigram1;
      std::string unigram2;
      ++index;

      textsim::tool::split_string_into_unigram1_unigram2(current, unigram1, unigram2);
      std::cout << "unigram1:\t" << unigram1 << "\tunigram2:\t" << unigram2 << std::endl;

      uint32_t id1;
      if (unigram1 != "grandpa") {
        id1 = (uint32_t) this->get_unigram_id(unigram1);
      } else {
        id1 = 6345254;
      }

      if (id1 == std::numeric_limits<uint32_t>::max()) {
        continue;
      }

      // std::cout<<"unigram 1 is \t"<<unigram1<<"\tid\t"<<id1<<std::endl;
      // std::cout<<"this->get_unigram_id"<<this->get_unigram_id(unigram2)<<std::endl;;

      uint32_t id2 = (uint32_t) this->get_unigram_id(unigram2);
      if (id2 == std::numeric_limits<uint32_t>::max()) {
        continue;
      }

      std::cout << "unigram 2 is \t" << unigram2 << "\tid\t" << id2 << std::endl;

      std::cout << "unigram1:\t" << unigram1 << "\tid:\t" << id1 << "\tunigram2:\t" << unigram2 << "\tid:\t" << id2
                << std::endl;

      auto digitnewkey = std::numeric_limits<uint32_t>::max() & ((id1 << textsim::common::IDWIDTH) | (id2));
      auto newkey = std::to_string(digitnewkey);
      const char *bikey = newkey.c_str();
      unsigned int biindex = cmph_search(this->hashbi, bikey, (cmph_uint32) strlen(bikey));
      std::bitset<IDWIDTH * 2> temp(digitnewkey);
      this->BIARRAY[biindex] = temp;
      check << current << "\t\t" << temp.to_ulong() << "\t\t" << digitnewkey << "\t\t" << biindex << std::endl;
    }
    std::cout << "cap of bi is " << index << std::endl;
    check.close();
  }

  void initcon(std::string contextset) {
    std::ifstream source;
    source.open(contextset, std::ios::in);

    if (!source.is_open()) {
      throw std::runtime_error("context dataset not valid");
    }

    std::ofstream check;
    check.open("/raid6/workspace/zichu/phsim/PhSim/build/src/concheck", std::ios::out);
    if (!check.is_open()) {
      throw std::runtime_error("conchek not open");
    }
    check << "ngram\t\tinside bitarray\t\tdigitkey\t\tpos" << std::endl;

    long index = 0;
    while (!source.eof()) {
      std::string current;
      std::getline(source, current);
      ++index;
      std::string unigram1;
      std::string unigram2;

      textsim::tool::split_string_into_unigram1_unigram2(current, unigram1, unigram2);

      uint_fast32_t id1 = (uint_fast32_t) this->get_unigram_id(unigram1);
      if (id1 == std::numeric_limits<uint32_t>::max()) {
        continue;
      }

      uint_fast32_t id2 = (uint_fast32_t) this->get_unigram_id(unigram2);
      if (id2 == std::numeric_limits<uint32_t>::max()) {
        continue;
      }

      auto digitnewkey = std::numeric_limits<uint32_t>::max() & ((id1 << textsim::common::IDWIDTH) | (id2));
      auto newkey = std::to_string(digitnewkey);
      const char *conkey = newkey.c_str();
      unsigned int conindex = cmph_search(this->hashcon, conkey, (cmph_uint32) strlen(conkey));
      std::bitset<IDWIDTH * 2> temp(digitnewkey);
      this->CONARRAY[conindex] = temp;
      check << current << "\t\t" << temp.to_ulong() << "\t\t" << digitnewkey << "\t\t" << conindex << std::endl;
    }

    std::cout << "cap of con is " << index << std::endl;
    check.close();
  }

  void serialize(std::string unipath, std::string bipath, std::string conpath) {
    std::ofstream targetuni;
    std::ofstream targetbi;
    std::ofstream targetcon;

    targetuni.open(unipath, std::ios::out);
    targetbi.open(bipath, std::ios::out);
    targetcon.open(conpath, std::ios::out);

    if (!targetuni.is_open() || !targetbi.is_open() || !targetcon.is_open()) {
      throw std::runtime_error("serialization not successful due to invalid path");
    }

    for (size_t i = 0; i < textsim::common::UNISIZE; ++i) {
      targetuni << this->uniarray[i] << std::endl;
    }

    std::cout << "uni serialization finished " << std::endl;

    for (size_t i = 0; i < textsim::common::BISIZE; ++i) {
      targetbi << this->BIARRAY[i].to_ulong() << std::endl;
    }

    std::cout << "bi serialization finished " << std::endl;

    for (size_t i = 0; i < textsim::common::CONSIZE; ++i) {
      targetcon << this->CONARRAY[i].to_ulong() << std::endl;
    }

    std::cout << "con serialization finished " << std::endl;

    targetuni.close();
    targetbi.close();
    targetcon.close();
  }

  void load(std::string unipath, std::string bipath, std::string conpath) {
    std::ifstream sourceuni;
    std::ifstream sourcebi;
    std::ifstream sourcecon;

    sourceuni.open(unipath, std::ios::in);
    sourcebi.open(bipath, std::ios::in);
    sourcecon.open(conpath, std::ios::in);

    if (!sourceuni.is_open() || !sourcebi.is_open() || !sourcecon.is_open()) {
      throw std::runtime_error("source path not valid");
    }

    for (size_t i = 0; i < textsim::common::UNISIZE; ++i) {
      std::string current;
      std::getline(sourceuni, current);
      this->uniarray[i] = current;
    }

    for (size_t i = 0; i < textsim::common::BISIZE; ++i) {
      std::string current;
      std::getline(sourcebi, current);
      if (current.length() < 1) {
        continue;
      }
      auto fingerprintval = std::stoul(current);
      std::bitset<textsim::common::IDWIDTH * 2> fingerprint(fingerprintval);
      this->BIARRAY[i] = fingerprint;
    }

    for (size_t i = 0; i < textsim::common::CONSIZE; ++i) {
      std::string current;
      std::getline(sourcecon, current);
      if (current.length() < 1) {
        continue;
      }
      auto fingerprintval = std::stoul(current);
      std::bitset<CombineRangeIndexer::IDWIDTH * 2> fingerprint(fingerprintval);
      this->CONARRAY[i] = fingerprint;
    }
  };

  virtual uint32_t get_unigram_id(std::string input) {
    if (input.length() < 1) {
      return std::numeric_limits<uint32_t>::max();
    }
    std::cout << "getting unigram id " << input << std::endl;
    const char *query = input.c_str();
    unsigned int id = cmph_search(this->hashuni, query, (cmph_uint32) strlen(query));
    std::cout << "after cmph search" << std::endl;
    std::cout << "id of this unigram is " << id << std::endl;
    std::string temp = this->uniarray[id];
    std::cout << "after assigning temp" << std::endl;
    if (temp == input) {
      return id;
    } else {
      return std::numeric_limits<uint32_t>::max();
    }
  };

  unsigned long bigramnumerical_generator(std::string input, uint32_t &id1, uint32_t &id2) {
    std::string unigram1;
    std::string unigram2;
    textsim::tool::split_string_into_unigram1_unigram2(input, unigram1, unigram2);

    id1 = (uint32_t) this->get_unigram_id(textsim::tool::trim(unigram1));
    id2 = (uint32_t) this->get_unigram_id(textsim::tool::trim(unigram2));

    uint_fast32_t longid1 = id1;
    uint_fast32_t longid2 = id2;

    if (longid1 == std::numeric_limits<uint32_t>::max() || longid2 == std::numeric_limits<uint32_t>::max()) {
      return std::numeric_limits<uint32_t>::max();
    } else {
      return ((longid1) | (longid2));
    }
  }

  virtual uint32_t get_bigram_id(std::string input) {
    std::string unigram1;
    std::string unigram2;
    textsim::tool::split_string_into_unigram1_unigram2(input, unigram1, unigram2);
    uint32_t id1 = (uint32_t) this->get_unigram_id(unigram1);
    uint32_t id2 = (uint32_t) this->get_unigram_id(unigram2);
    if (id1 == std::numeric_limits<uint32_t>::max() || id2 == std::numeric_limits<uint32_t>::max()) {
      return std::numeric_limits<uint32_t>::max();
    } else {
      auto bikeynum = std::numeric_limits<uint32_t>::max() & ((id1 << textsim::common::IDWIDTH) | (id2));
      std::string bikeystr = std::to_string(bikeynum);
      const char *bikey = bikeystr.c_str();
      unsigned int biid = cmph_search(hashbi, bikey, (cmph_uint32) strlen(bikey));
      unsigned long realindex = this->BIARRAY[biid].to_ulong();
      if (realindex == bikeynum) {
        return biid + textsim::common::UNISIZE;
      } else {
        biid = cmph_search(this->hashcon, bikey, (cmph_uint32) strlen(bikey));
        realindex = this->CONARRAY[biid].to_ulong();
        if (realindex == bikeynum) {
          return biid + textsim::common::UNISIZE + textsim::common::BISIZE;
        } else {
          return std::numeric_limits<uint32_t>::max();
        }
      }
    }
  };
};
};
#endif //TEXTSIM_COMBINERANGEINDEXER_HPP
