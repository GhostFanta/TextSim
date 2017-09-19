#ifndef TEXTSIM_NAIVEGIINDEXER_HPP
#define TEXTSIM_NAIVEGIINDEXER_HPP
#include <cmph.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <exception>
#include <thread>
#include <stdio.h>

#include "interfaces/giindexer.hpp"

namespace textsim{
class MultiIndexer: public GIIndexer{
 private:
  std::vector<std::string> dic;
  std::string mphpath;
  cmph_t *hash;
  FILE *mphfile;
  unsigned int KEYSIZE;
 private:
  void static buildindexer(std::string keysetpartpath,cmph_t *hash,std::vector<std::string> *dic){
    char *line;
    size_t len = 0;
    ssize_t read;
    const char *keysetpath = &keysetpartpath[0u];
    FILE *keyset  = fopen(keysetpath,"r");
    if(keyset == NULL){
      throw std::runtime_error("the keyset part is not valid");
    }
    if(hash  ==  NULL){
      throw std::runtime_error("the hash function is not ready");
    }
    while((read = getline(&line, &len, keyset)) != -1){
      line[strlen(line)-1] = '\0';
      unsigned int id =  cmph_search(hash ,line, (cmph_uint32) strlen(line));
      (*dic)[id]= std::string(line);
    }
    std::cout<<"indexer chunk finished "<<std::endl;
  }

 public:
  MultiIndexer(std::string mphpathinput,unsigned int keysetsize){
    this->KEYSIZE = keysetsize;
    this->dic.resize(this->KEYSIZE);
    const char *mphpath = &mphpathinput[0u];
    mphfile = fopen(mphpath,"r");
    this->hash = cmph_load(mphfile);
  };

  MultiIndexer(std::string serializedfile){

  }

  void initindexer(std::vector<std::string> keysetpartpathinput,std::string serializedfilepath){

    std::vector<std::thread> threads;
    for(auto i : keysetpartpathinput){
      std::vector<std::string> *tempdic = &this->dic;
      threads.emplace_back(std::thread(MultiIndexer::buildindexer,i,this->hash,tempdic));
    }

    for(auto &i : threads){
      i.join();
    }

    std::ofstream target;
    target.open(serializedfilepath,std::ios::out);
    if(!target.is_open()){
      throw std::runtime_error("the target path is not valid");
    }
  }

  std::vector<std::string> * testindexer(){
    std::vector<std::string> *res = &(this->dic);
    return res;
  }

  id_t get_unigram_id(std::string input) {
    const char *query = input.c_str();
    unsigned int id = cmph_search(this->hash, query, (cmph_uint32)strlen (query));
    std::string temp = this->dic[id];
    if (temp == input) {
      return id;
    } else {
      return std::numeric_limits<id_t>::max();
    }
  };

  id_t get_bigram_id(std::string input) {
    try{
      const char *query = input.c_str();
      unsigned int id = cmph_search(this->hash, query, (cmph_uint32)strlen (query));
      std::string temp = this->dic[id];
      if (temp == input) {
        return id;
      } else {
        return std::numeric_limits<id_t>::max();
      }
    }catch(std::runtime_error& e){
      std::cout<<e.what()<<std::endl;
    }
  };
};
}
#endif //TEXTSIM_NAIVEGIINDEXER_HPP
