#ifndef TEXTSIM_PROCESS_HPP
#define TEXTSIM_PROCESS_HPP

#include "common.hpp"
#include <queue>
#include "giindexer/naivegiindexer.hpp"
#include "giindexer/combinerangeindexer.hpp"

namespace process {

std::vector<std::string> unigramcontentpath = {"/raid6/workspace/zichu/rakibng/allUnigrams"};
std::string unigrammphpath = "/raid6/workspace/zichu/rakibng/allUnigrams.mph";
std::string unigramwithfreqpath = "/raid6/workspace/zichu/rakibng/1gm/formattedUnigrams";
std::string unigramserializationpath = "/raid6/workspace/zichu/rakibng/serialization/uniindexerserialization";

std::string bigramcontentpath = "/raid6/workspace/zichu/rakibng/allBigrams";
std::string bigrammphpath = "/raid6/workspace/zichu/rakibng/bigramIndexOut.mph";
std::string bigramwithfreqpath = "/raid6/workspace/zichu/rakibng/2gm/formattedBigrams";
std::string bigramserializationpath = "/raid6/workspace/zichu/rakibng/serialization/biindexerserialization";

std::string contextcontentpath = "/raid6/workspace/zichu/rakibng/allContextsPureTrim";
std::string contextmphpath = "/raid6/workspace/zichu/rakibng/allContextPureTrimIndexOut.mph";
std::string contextserializationpath = "/raid6/workspace/zichu/rakibng/serialization/conindexerserialization";

std::string stopwordfile = "/raid6/workspace/zichu/phsim/PhSim/resources/stopwords";

void test_naive_indexer() {

};

void test_tieredindexer() {
  textsim::CombineRangeIndexer *combineindexer = new textsim::CombineRangeIndexer(unigrammphpath, bigrammphpath);
  combineindexer->initcmphbi(bigrammphpath);
  combineindexer->initcmphcon(contextmphpath);
  combineindexer->init(unigramcontentpath);
  std::cout << "unigram inited" << std::endl;
  combineindexer->initcon(contextcontentpath);
  std::cout << "context inited" << std::endl;
  combineindexer->initbi(bigramcontentpath);
  std::cout << "bigram inited" << std::endl;

  combineindexer->serialize(unigramserializationpath, bigramserializationpath, contextserializationpath);
  std::cout << "indexer serialization finished" << std::endl;
  std::cout << "indexer init finished" << std::endl;

  std::priority_queue<uint32_t> uniqueue;
  std::priority_queue<uint32_t> biqueue;
  std::priority_queue<uint32_t> conqueue;

  std::ifstream source;

  source.open(unigramcontentpath[0], std::ios::in);
  if (!source.is_open()) {
    throw std::runtime_error("unigram dataset not valid");
  }

  while (!source.eof()) {
    std::string current;
    std::getline(source, current);
    auto id = combineindexer->get_unigram_id(current);
    if (id != std::numeric_limits<uint32_t>::max())
      uniqueue.push(id);
  }
  std::cout << "uniqueue push finish" << std::endl;

  source.close();

  source.open(bigramcontentpath, std::ios::in);
  if (!source.is_open()) {
    throw std::runtime_error("bigram dataset not valid");
  }

  while (!source.eof()) {
    std::string current;
    std::getline(source, current);
    auto id = combineindexer->get_bigram_id(current);
    if (id != std::numeric_limits<uint32_t>::max()) {
      biqueue.push(id);
    }
  }

  std::cout << "biqueue push finish" << std::endl;

  source.close();

  source.open(contextcontentpath, std::ios::in);
  if (!source.is_open()) {
    throw std::runtime_error("context dataset not valid");
  }

  while (!source.eof()) {
    std::string current;
    std::getline(source, current);
    combineindexer->get_bigram_id(current);
    auto id = combineindexer->get_bigram_id(current);
    if (id != std::numeric_limits<uint32_t>::max()) {
      conqueue.push(id);
    }
  }

  std::cout << "con push finish" << std::endl;

  std::ofstream uni;
  std::ofstream bi;
  std::ofstream con;

  uni.open("/raid6/workspace/zichu/phsim/PhSim/build/src/unirange", std::ios::out);
  bi.open("/raid6/workspace/zichu/phsim/PhSim/build/src/birange", std::ios::out);
  con.open("/raid6/workspace/zichu/phsim/PhSim/build/src/conrange", std::ios::out);

  if (!uni.is_open() || !bi.is_open() || !con.is_open()) {
    throw std::runtime_error("target not valid");
  }

  while (!uniqueue.empty()) {
    uni << uniqueue.top() << std::endl;
    uniqueue.pop();
  }

  std::cout << "uniqueue pop finish" << std::endl;

  while (!biqueue.empty()) {
    bi << biqueue.top() << std::endl;
    biqueue.pop();
  }

  std::cout << "biqueue pop finish" << std::endl;

  while (!conqueue.empty()) {
    con << conqueue.top() << std::endl;
    conqueue.pop();
  }

  std::cout << "conqueue pop finish" << std::endl;

  uni.close();
  bi.close();
  con.close();
};

void test_naive_gfindexer() {

};

void test_aligned_gfindexer() {

};

void test_compact_gfindexer() {

}

}

#endif //TEXTSIM_PROCESS_HPP
