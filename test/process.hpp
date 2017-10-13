#ifndef TEXTSIM_PROCESS_HPP
#define TEXTSIM_PROCESS_HPP

#include "common.hpp"
#include <queue>
#include "giindexer/naivegiindexer.hpp"
#include "giindexer/combinerangeindexer.hpp"
#include "gfindexer/naivegfindexer.hpp"
#include "gfindexer/compactgfindexer.hpp"
#include "gfindexer/alignedgfindexer.hpp"
#include "interfaces/stopwords.hpp"
#include "stopwords/unorderedstopwords.hpp"

#include "interfaces/intermediate_x8.hpp"
#include "interfaces/intermediate_x32.hpp"
#include "interfaces/intermediate_x64.hpp"

#include "bytecodec/variantgb.hpp"
#include "bytecodec/variantbyte.hpp"

#include "interfaces/gcindexer.hpp"
#include "gcindexer/trigram/invertedgctri.hpp"

namespace process {

std::vector<std::string> unigramcontentpath = {"/raid6/workspace/zichu/rakibng/allUnigrams"};
std::string unigrammphpath = "/raid6/workspace/zichu/rakibng/allUnigrams.mph";
std::string unigramwithfreqpath = "/raid6/workspace/zichu/rakibng/1gm/formattedUnigrams";
std::string unigramserializationpath = "/raid6/workspace/zichu/rakibng/serialization/uniindexerserialization";


std::string opunigrammphpath = "/raid6/workspace/zichu/rakibng/UnigramSet.mph";
std::string opunigramcontentpath = "/raid6/workspace/zichu/rakibng/UnigramSet";
std::string targetunicontentpath = "/raid6/workspace/zichu/rakibng/targetUnigrams";
std::string otherunicontentpath = "/raid6/workspace/zichu/rakibng/otherUnigrams";

std::string bigramcontentpath = "/raid6/workspace/zichu/rakibng/allBigrams";
std::string bigrammphpath = "/raid6/workspace/zichu/rakibng/serialization/bigramThumbNails.mph";
std::string bigramwithfreqpath = "/raid6/workspace/zichu/rakibng/2gm/formattedBigrams";
std::string bigramserializationpath = "/raid6/workspace/zichu/rakibng/serialization/biindexerserialization";
std::string bigramthumbnailpath = "/raid6/workspace/zichu/rakibng/serialization/bigramThumbNails";

std::string contextcontentpath = "/raid6/workspace/zichu/rakibng/allContextsPureTrim";
std::string contextmphpath = "/raid6/workspace/zichu/rakibng/serialization/contextThumnails.mph";
std::string contextserializationpath = "/raid6/workspace/zichu/rakibng/serialization/conindexerserialization";
std::string contextthumbnailpath = "/raid6/workspace/zichu/rakibng/serialization/contextThumnails";

std::vector<std::string> trigramcontextarrayvec = {
    "/raid6/workspace/zichu/rakibng/3gm/digital"
};

std::string stopwordfile = "/raid6/workspace/zichu/phsim/PhSim/resources/stopwords";

void test_splited_uni_mph(){
  FILE *mphfile;
  mphfile = fopen(opunigrammphpath.c_str(),"r");
  cmph_t *hash;
  hash = cmph_load(mphfile);
  char *line;
  size_t len = 0;
  ssize_t read;
  // const char *keysetpath = targetunicontentpath.c_str();
  const char *keysetpath = otherunicontentpath.c_str();
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
    std::cout<<id<<std::endl;
  }
  std::cout<<"indexer chunk finished "<<std::endl;
}

void test_naive_indexer() {
  std::vector<std::string> keysetpathvec;

  keysetpathvec.push_back("/raid6/workspace/zichu/rakibng/allKeys");

  std::string mphpath = "/raid6/workspace/zichu/rakibng/allKeys.mph";
  unsigned int keysetcapacity = 198820259;

  std::string stopwordfile = "/raid6/workspace/zichu/phsim/PhSim/resources/stopwords";

  std::cout<<"after stopwordfile"<<std::endl;

  textsim::MultiIndexer *multiindexer = new textsim::MultiIndexer(mphpath,keysetcapacity);
  multiindexer->initindexer(keysetpathvec,"/raid6/workspace/zichu/phsim/stat");
  textsim::GIIndexer *indexer = multiindexer;


  std::ifstream sourceuni;
  std::ifstream sourcebi;
  std::ofstream target;

  sourceuni.open("/raid6/workspace/zichu/rakibng/allUnigrams",std::ios::in);
  sourcebi.open("/raid6/workspace/zichu/rakibng/allBigrams",std::ios::in);

  target.open("/raid6/workspace/zichu/rakibng/targetkey",std::ios::out);

  if(!sourceuni.is_open() || !sourcebi.is_open()){
    throw std::runtime_error("source not valid");
  }
  std::priority_queue<uint32_t> keyqueue;

  while(!sourceuni.eof()){
    std::string current;
    std::getline(sourceuni,current);
    if(current.length() < 1){
      break;
    }
    keyqueue.push(multiindexer->get_unigram_id(current));
  }

  while(!sourcebi.eof()){
    std::string current;
    std::getline(sourcebi,current);
    if(current.length()<1){
      break;
    }
    keyqueue.push(multiindexer->get_bigram_id(current));
  }

  while(!keyqueue.empty()){
    target<<keyqueue.top()<<std::endl;
    keyqueue.pop();
  }

  sourceuni.close();
  sourcebi.close();
  target.close();
};

void test_correctness_bigram_tiered_giindexer(){
  textsim::CombineRangeIndexer *combineindexer = new textsim::CombineRangeIndexer(unigrammphpath, bigrammphpath);
  combineindexer->initcmphbi(bigrammphpath);
  combineindexer->initcmphcon(contextmphpath);
  combineindexer->init(unigramcontentpath);
  std::cout << "unigram inited" << std::endl;
  combineindexer->initbi(bigramcontentpath);
  std::cout << "bigram inited" << std::endl;

  std::cout<<combineindexer->get_bigram_id("gilf hot")<<std::endl;
  std::cout<<combineindexer->get_bigram_id("with restek")<<std::endl;

}

void gen_tieredindexer_bigram_context_thumbnail(){
  textsim::CombineRangeIndexer *combineRangeIndexer = new textsim::CombineRangeIndexer(opunigrammphpath,bigrammphpath);
  std::vector<std::string> temp;
  temp.push_back(opunigramcontentpath);
  combineRangeIndexer->init(temp);
  std::ifstream sourcebi;
  std::ofstream targetbi;
  std::ofstream checkbi;
  sourcebi.open(bigramcontentpath,std::ios::in);
  targetbi.open(bigramthumbnailpath,std::ios::out);
  checkbi.open("/raid6/workspace/zichu/rakibng/serialization/checkbi");
  if(!targetbi.is_open() || !sourcebi.is_open() || !checkbi.is_open()){
    throw std::runtime_error("bigramthumbnailpath not valid");
  }



  while(!sourcebi.eof()){
    std::string current;
    std::getline(sourcebi,current);
    uint32_t id1;
    uint32_t id2;
    targetbi<<combineRangeIndexer-> bigramnumerical_generator(current,id1,id2);
    targetbi<<std::endl;
    checkbi<<current<<std::endl;
    checkbi<<id1<<std::endl;
    checkbi<<id2<<std::endl;
    checkbi<<combineRangeIndexer-> bigramnumerical_generator(current,id1,id2);
  }


  std::ifstream sourcecon;
  std::ofstream targetcon;
  std::ofstream checkcon;
  sourcecon.open(contextcontentpath,std::ios::in);
  targetcon.open(contextthumbnailpath,std::ios::out);
  checkcon.open("/raid6/workspace/zichu/rakibng/serialization/checkcon");
  if(!targetcon.is_open() || !sourcecon.is_open() || !checkcon.is_open()){
    throw std::runtime_error("contextthumbnailpath not valid");
  }

  while(!sourcecon.eof()){
    std::string current;
    std::getline(sourcecon,current);
    uint32_t id1;
    uint32_t id2;
    combineRangeIndexer-> bigramnumerical_generator(current,id1,id2);
    targetcon<<combineRangeIndexer->bigramnumerical_generator(current,id1,id2);
    targetcon<<std::endl;
    checkcon<<current<<std::endl;
    checkcon<<id1<<std::endl;
    checkcon<<id2<<std::endl;
    checkcon<<combineRangeIndexer->bigramnumerical_generator(current,id1,id2);
  }

  sourcebi.close();
  sourcecon.close();
  checkbi.close();
  targetbi.close();
  targetcon.close();
  checkcon.close();

  std::cout<<"thumbnail generation finish"<<std::endl;
}

// Two versions of unigrams attention!!!!
void test_tieredindexer() {
  textsim::CombineRangeIndexer *combineindexer = new textsim::CombineRangeIndexer(opunigrammphpath, bigrammphpath);
  combineindexer->initcmphbi(bigrammphpath);
  combineindexer->initcmphcon(contextmphpath);
  std::vector<std::string> temp;
  temp.push_back(opunigramcontentpath);
  combineindexer->init(temp);
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
    // std::cout<<current<<"\t"<<id<<std::endl;
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
    std::cout<<current<<"\t"<<id<<std::endl;
    if (id != std::numeric_limits<uint32_t>::max()) {
      biqueue.push(id);
    }else{
      std::cout<<"bigram id not valid:\t"<<current<<std::endl;
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
    std::cout<<current<<"\t"<<id<<std::endl;
    if (id != std::numeric_limits<uint32_t>::max()) {
      std::cout<<"context id not valid:\t"<<current<<std::endl;
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

void test_tieredindexer_with_load(){
  textsim::CombineRangeIndexer *combineRangeIndexer = new textsim::CombineRangeIndexer(unigrammphpath,bigrammphpath);
  combineRangeIndexer->initcmphbi(bigrammphpath);
  combineRangeIndexer->initcmphcon(contextmphpath);
  combineRangeIndexer->init(unigramcontentpath);

  combineRangeIndexer->load(unigramserializationpath,bigramserializationpath,contextserializationpath);

  uint32_t id1;
  uint32_t id2;

  std::cout<<combineRangeIndexer->get_unigram_id("ma")<<std::endl;
  std::cout<<combineRangeIndexer->get_unigram_id("senatori")<<std::endl;
  std::cout<<combineRangeIndexer->bigramnumerical_generator("ma senatori",id1,id2)<<std::endl;;
  std::cout<<combineRangeIndexer->get_bigram_id("ma senatori")<<std::endl;

  std::cout<<combineRangeIndexer->get_unigram_id("upcom")<<std::endl;
  std::cout<<combineRangeIndexer->get_unigram_id("nortel")<<std::endl;
  std::cout<<combineRangeIndexer->bigramnumerical_generator("upcom nortel",id1,id2)<<std::endl;
  std::cout<<combineRangeIndexer->get_bigram_id("upcom nortel")<<std::endl;

  std::cout<<"womenswear\t"<<combineRangeIndexer->get_unigram_id("womenswear");
  std::cout<<"appoint\t"<<combineRangeIndexer->get_unigram_id("appoint");
  std::cout<<"womenswear appoint\t"<<combineRangeIndexer->bigramnumerical_generator("womenswear appoint",id1,id2)<<std::endl;;
  std::cout<<""<<combineRangeIndexer->get_bigram_id("womenswear appoint")<<std::endl;

  std::cout<<combineRangeIndexer->get_unigram_id("gilf")<<std::endl;
  std::cout<<combineRangeIndexer->get_unigram_id("hot")<<std::endl;
  std::cout<<combineRangeIndexer->get_unigram_id("ingl")<<std::endl;
  std::cout<<combineRangeIndexer->get_unigram_id("ha")<<std::endl;


  std::cout<<combineRangeIndexer->get_bigram_id("gilf hot")<<std::endl;
  std::cout<<combineRangeIndexer->get_bigram_id("logitech lowepro")<<std::endl;

};

void test_naive_gfindexer() {

  std::string unimphpath = unigrammphpath;
  std::string bimphpath = bigrammphpath;
  std::string conmphpath = contextmphpath;

  std::string stopwordfile = "/raid6/workspace/zichu/phsim/PhSim/resources/stopwords";
  std::cout << "after stopwordfile" << std::endl;

  std::cout << "hadoop perfect fun finished " << std::endl;

  std::string bigramspath = bigramwithfreqpath;
  std::string contextspath = contextcontentpath;

  textsim::CombineRangeIndexer *combineindexer = new textsim::CombineRangeIndexer(unimphpath, bimphpath);
  combineindexer->initcmphbi(bimphpath);
  combineindexer->initcmphcon(conmphpath);

  combineindexer->load(unigramserializationpath,bigramserializationpath,contextserializationpath);

  std::vector<std::string> unifreqchunk;
  std::vector<std::string> bifreqchunk;
  unifreqchunk.push_back(unigramwithfreqpath);
  bifreqchunk.push_back(bigramwithfreqpath);

  std::cout << "construction of giindexer finished " << std::endl;


  std::vector<std::string> bigramfreqvec;
  bigramfreqvec.push_back(unigramwithfreqpath);
  bigramfreqvec.push_back(bigramwithfreqpath);

  auto t1 = std::chrono::high_resolution_clock::now();
  textsim::MultiBigram *bigram = new textsim::MultiBigram(combineindexer);
  bigram->initbigram(bigramfreqvec);
  auto t2 = std::chrono::high_resolution_clock::now();
  std::cout << "gfindexer construction takes " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
            << " microseconds";

  std::ifstream freqsourceuni;
  std::ifstream freqsourcebi;

  freqsourcebi.open(bigramwithfreqpath,std::ios::in);
  freqsourceuni.open(unigramwithfreqpath,std::ios::in);

  if(!freqsourceuni.is_open()||!freqsourcebi.is_open()){
    throw std::runtime_error("freq source file is not valid");
  }

  size_t numofgram = 0;

  size_t totalretrievetime = 0;



  while(!freqsourceuni.eof()){
    std::string current;
    std::getline(freqsourceuni, current);
    if(current.length()<1){
      break;
    }
    ++numofgram;
    std::string gram;
    uint64_t freq;
    textsim::tool::split_string_into_gram_freq(current,gram,freq);
    auto t3 = std::chrono::high_resolution_clock::now();
    auto retfreq = bigram->get_freq(gram);
    auto t4 = std::chrono::high_resolution_clock::now();
    totalretrievetime+=std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count();
    assert(retfreq == freq);
  }


  while(!freqsourcebi.eof()){
    std::string current;
    std::getline(freqsourcebi ,current);
    if(current.length()<1){
      break;
    }
    ++numofgram;
    std::string gram;
    uint64_t freq;
    textsim::tool::split_string_into_gram_freq(current,gram,freq);
    auto t3 = std::chrono::high_resolution_clock::now();
    auto retfreq = bigram->get_freq(gram);
    auto t4 = std::chrono::high_resolution_clock::now();
    totalretrievetime+=std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count();
    assert(bigram->get_freq(gram) == freq);
  }
  std::cout << "gfindexer retrieval\t" <<numofgram<<"takes " << totalretrievetime
            << " nanoseconds";
};

void test_aligned_gfindexer() {

  std::string stopwordfile = "/raid6/workspace/zichu/phsim/PhSim/resources/stopwords";
  std::cout<<"after stopwordfile"<<std::endl;

  textsim::CombineRangeIndexer *combineRangeIndexer = new textsim::CombineRangeIndexer(unigrammphpath,bigrammphpath);
  combineRangeIndexer->initcmphbi(bigrammphpath);
  combineRangeIndexer->initcmphcon(contextmphpath);
  combineRangeIndexer->init(unigramcontentpath);

  combineRangeIndexer->load(unigramserializationpath,bigramserializationpath,contextserializationpath);

  std::vector<std::string> unifreqchunk;
  std::vector<std::string> bifreqchunk;
  unifreqchunk.push_back(unigramwithfreqpath);
  bifreqchunk.push_back(bigramwithfreqpath);

  textsim::CompressedmixGFIndexer *gfindexer = new textsim::CompressedmixGFIndexer(combineRangeIndexer);
  gfindexer->inituni(unifreqchunk);
  std::cout<<"gf uni finish"<<std::endl;
  gfindexer->initbi(bifreqchunk);
  std::cout<<"gf bi finish"<<std::endl;

  std::ifstream source;
  source.open(unigramwithfreqpath,std::ios::in);

  unsigned long match = 0;
  unsigned long exception = 0;

  if(!source.is_open()){
    throw std::runtime_error("1gmfreq not valid");
  }

  unsigned long index = 0;
  unsigned long getlineinterval = 0;
  unsigned long splitinterval = 0;
  unsigned long getfreqinterval = 0;

  while(!source.eof()){
    ++index;
    auto t1 = std::chrono::high_resolution_clock::now();
    std::string current;
    std::getline(source,current);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::string gram;
    unsigned long freq;
    if(current.length()<2){
      continue;
    }
    textsim::tool::split_string_into_gram_freq(current,gram,freq);
    auto t3 = std::chrono::high_resolution_clock::now();
    if(freq == gfindexer->get_freq(gram)){
      ++match;
    }else{
      ++exception;
    }
    auto t4 = std::chrono::high_resolution_clock::now();
    getlineinterval += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    splitinterval += std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
    getfreqinterval += std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
    if(index % 1000000 == 0){
      std::cout<<"getlineinterval : "<<getlineinterval<<std::endl;
      std::cout<<"splitinterval : "<<splitinterval<<std::endl;
      std::cout<<"getfreqinterval: "<<getfreqinterval<<std::endl;
      getlineinterval = 0;
      splitinterval = 0;
      getfreqinterval= 0;
    }

  }
  source.close();

  std::cout<<"unigram check ready"<<std::endl;

  source.open(bigramwithfreqpath, std::ios::in);
  if(!source.is_open()){
    throw std::runtime_error("2gmfreqchunk not valid");
  }
  while(!source.eof()){
    std::string current;
    std::getline(source,current);
    std::string gram;
    unsigned long freq;
    if(current.length()<2){
      continue;
    }
    textsim::tool::split_string_into_gram_freq(current,gram,freq);
    if(freq == gfindexer->get_freq(gram)){
      ++match;
    }else{
      ++exception;
    }
  }
  std::cout<<"matches: "<<match<<std::endl;
  std::cout<<"exception: "<<exception<<std::endl;
};

void test_compact_gfindexer() {
  std::vector<std::string> keysetpathvec;
  keysetpathvec.push_back("/raid6/workspace/zichu/hadoopkeyset/stemmedversion/1gms/1gmfromallstem");

  std::string unifile = unigramserializationpath;
  std::string bifile = bigramserializationpath;
  std::string confile = contextserializationpath;
  std::string unimphpath = opunigrammphpath;
  std::string bimphpath = bigrammphpath;
  std::string conmphpath = contextmphpath;
  std::string bigramspath = bigramcontentpath;
  std::string contextspath = contextcontentpath;

  textsim::CombineRangeIndexer *combineindexer = new textsim::CombineRangeIndexer(unimphpath, bimphpath);
  combineindexer->initcmphbi(bimphpath);
  combineindexer->initcmphcon(conmphpath);
  combineindexer->load(unifile, bifile, confile);

  std::cout << "indexer load finish" << std::endl;


  auto t1 = std::chrono::high_resolution_clock::now();
  std::string unipath =unigramwithfreqpath;
  std::string bipath = bigramwithfreqpath;
  textsim::CompressedOptimizedGFIndexer *gfindexer = new textsim::CompressedOptimizedGFIndexer(combineindexer);
  gfindexer->inituni(unipath);
  std::cout << "load uni finished" << std::endl;
  gfindexer->initbi(bipath);
  std::cout << "load bi finished" << std::endl;
  auto t2 = std::chrono::high_resolution_clock::now();
  gfindexer->clear();
  std::cout << "clear" << std::endl;

  std::cout << "gfindexer load finish\t" << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
            << std::endl;

  size_t match = 0;
  size_t mismatch = 0;
  std::ifstream sourceuni;
  std::ifstream sourcebi;

  sourceuni.open(unipath, std::ios::in);
  sourcebi.open(bipath, std::ios::in);

  if (!sourceuni.is_open() || !sourcebi.is_open()) {
    throw std::runtime_error("unipath or bipath not valid");
  };

  while (!sourceuni.eof()) {
    std::string current;
    std::getline(sourceuni, current);
    if (current.length() < 1) {
      break;
    }
    std::string ngram;
    unsigned long freq;
    textsim::tool::split_string_into_gram_freq(current, ngram, freq);
    std::cout << "ngram is\t" << ngram << std::endl;
    if (gfindexer->get_freq(ngram) == freq) {
      ++match;
    } else {
      ++mismatch;
    }
  };

  while (!sourcebi.eof()) {
    std::string current;
    std::getline(sourcebi, current);
    if (current.length() < 1) {
      break;
    }
    std::string ngram;
    unsigned long freq;
    textsim::tool::split_string_into_gram_freq(current, ngram, freq);
    std::cout << "ngram is\t" << ngram << std::endl;
    if (gfindexer->get_freq(ngram) == freq) {
      ++match;
    } else {
      ++mismatch;
    }
  };
  std::cout << "matches: \t" << match << std::endl;
  std::cout << "mismatch: \t" << mismatch << std::endl;
}

void test_invertedtri_gcindexer(){
  std::vector<std::string> keysetpathvec;
  keysetpathvec.push_back("/raid6/workspace/zichu/hadoopkeyset/stemmedversion/1gms/1gmfromallstem");

  std::string unifile = unigramserializationpath;
  std::string bifile = bigramserializationpath;
  std::string confile = contextserializationpath;
  std::string unimphpath = opunigrammphpath;
  std::string bimphpath = bigrammphpath;
  std::string conmphpath = contextmphpath;
  std::string bigramspath = bigramcontentpath;
  std::string contextspath = contextcontentpath;

  textsim::CombineRangeIndexer *combineindexer = new textsim::CombineRangeIndexer(unimphpath, bimphpath);
  combineindexer->initcmphbi(bimphpath);
  combineindexer->initcmphcon(conmphpath);
  combineindexer->load(unifile, bifile, confile);

  std::cout << "indexer load finish" << std::endl;


  auto t1 = std::chrono::high_resolution_clock::now();
  std::string unipath =unigramwithfreqpath;
  std::string bipath = bigramwithfreqpath;
  textsim::CompressedOptimizedGFIndexer *gfindexer = new textsim::CompressedOptimizedGFIndexer(combineindexer);
  gfindexer->inituni(unipath);
  std::cout << "load uni finished" << std::endl;
  gfindexer->initbi(bipath);
  std::cout << "load bi finished" << std::endl;
  auto t2 = std::chrono::high_resolution_clock::now();
  gfindexer->clear();
  std::cout << "clear" << std::endl;

  std::cout << "gfindexer load finish\t" << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
            << std::endl;

  textsim::stopwords *stopwords = new textsim::stopwordsdic(stopwordfile);
  textsim::intermediate_x8 *codec = new textsim::vbyte();

  textsim::InvertedGCIndexerTri_x8 *gcindexer = new textsim::InvertedGCIndexerTri_x8(combineindexer,codec,stopwords);

  gcindexer->initchunks(trigramcontextarrayvec);

  gcindexer->converttodelta();

  gcindexer->compress();

  gcindexer->clearcache();

  std::vector<std::pair<uint64_t,uint64_t>> contextarray;

//  gcindexer->get_contexts("aaaac");

}

}

#endif //TEXTSIM_PROCESS_HPP