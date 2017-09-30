#ifndef TEXTSIM_UNORDEREDSTOPWORDS_HPP
#define TEXTSIM_UNORDEREDSTOPWORDS_HPP

#include "common.hpp"
#include "interfaces/stopwords.hpp"

namespace textsim{
class stopwordsdic:public stopwords{
 private:
  std::unordered_set<std::string> dic;
 public:
  stopwordsdic(std::string stopwordsfile){
    std::ifstream source;
    source.open();

    if(!source.is_open()){
      throw std::runtime_error("stopword source not valid");
    }

    while(!source.eof()){
      std::string current;
      std::getline(source,current);
      this->dic.insert(current);
    }
  }

  ~stopwordsdic(){};

  bool is_stopword(std::string input){
    return this->dic.find(input) == this->dic.end();
  };
};
};
#endif //TEXTSIM_UNORDEREDSTOPWORDS_HPP
