#ifndef TEXTSIM_PROCESS_HPP
#define TEXTSIM_PROCESS_HPP

#include "common.hpp"
#include <queue>
#include "giindexer/naivegiindexer.hpp"
#include "giindexer/combinerangeindexer.hpp"
#include "gfindexer/naivegfindexer.hpp"
#include "gfindexer/compactgfindexer.hpp"
#include "gfindexer/alignedgfindexer.hpp"

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
  std::vector<std::string> keysetpathvec;

  keysetpathvec.push_back("/raid6/workspace/zichu/rakibng/allKeys");

  std::string mphpath = "/raid6/workspace/zichu/rakibng/allKeys.mph";
  unsigned int keysetcapacity = 198820259;

  std::string stopwordfile = "/raid6/workspace/zichu/phsim/PhSim/resources/stopwords";

  std::cout<<"after stopwordfile"<<std::endl;

  textsim::MultiIndexer *multiindexer = new textsim::MultiIndexer(mphpath,keysetcapacity);
  multiindexer->initindexer(keysetpathvec,"/raid6/workspace/zichu/phsim/stat");
  textsim::GIIndexer *indexer = multiindexer;
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

//Pass
void test_tieredindexer_with_load(){
  textsim::CombineRangeIndexer *combineRangeIndexer = new textsim::CombineRangeIndexer(unigrammphpath,bigrammphpath);
  combineRangeIndexer->initcmphbi(bigrammphpath);
  combineRangeIndexer->initcmphcon(contextmphpath);
  combineRangeIndexer->init(unigramcontentpath);

  combineRangeIndexer->load(unigramserializationpath,bigramserializationpath,contextserializationpath);

  std::cout<<combineRangeIndexer->get_unigram_id("ozitape")<<std::endl;
  std::cout<<combineRangeIndexer->get_unigram_id("bosweath")<<std::endl;
  std::cout<<combineRangeIndexer->get_unigram_id("popolazion")<<std::endl;
  std::cout<<combineRangeIndexer->get_unigram_id("conversionvxi")<<std::endl;

  std::cout<<combineRangeIndexer->get_unigram_id("selelc")<<std::endl;
  std::cout<<combineRangeIndexer->get_unigram_id("awdhesh")<<std::endl;


  std::cout<<combineRangeIndexer->get_bigram_id("ma senatori")<<std::endl;
  std::cout<<combineRangeIndexer->get_bigram_id("upcom nortel")<<std::endl;
  std::cout<<combineRangeIndexer->get_bigram_id("gilf hot")<<std::endl;
  std::cout<<combineRangeIndexer->get_bigram_id("logitech lowepro")<<std::endl;

};

void test_naive_gfindexer() {
    std::vector<std::string> keysetpathvec;

    keysetpathvec.push_back("/raid6/workspace/zichu/hadoopkeyset/stemmedversion/1gms/1gmfromallstem");

    std::string unimphpath = "/raid6/workspace/zichu/hadoopkeyset/stemmedversion/1gms/1gmfromallstem.mph";
    std::string bimphpath = "/raid6/workspace/zichu/hadoopkeyset/stemmedversion/bigramindexoutpure0726.mph";
    std::string conmphpath = "/raid6/workspace/zichu/hadoopkeyset/stemmedversion/contextindexoutpure0726.mph";

    std::string stopwordfile = "/raid6/workspace/zichu/phsim/PhSim/resources/stopwords";
    std::cout << "after stopwordfile" << std::endl;

    std::cout << "hadoop perfect fun finished " << std::endl;

    std::string bigramspath = "/raid6/workspace/zichu/hadoopkeyset/stemmedversion/2gms/2gmstempure";
    std::string contextspath = "/raid6/workspace/zichu/hadoopkeyset/stemmedversion/2gms/contextsstempure";

    textsim::CombineRangeIndexer *combineindexer = new textsim::CombineRangeIndexer(unimphpath, bimphpath);
    combineindexer->initcmphbi(bimphpath);
    combineindexer->initcmphcon(conmphpath);

    combineindexer->init(keysetpathvec);
    combineindexer->initbi(bigramspath);
    combineindexer->initcon(contextspath);

    std::vector<std::string> unifreqchunk;
    std::vector<std::string> bifreqchunk;
    unifreqchunk.push_back("/raid6/workspace/zichu/hadoopkeyset/stemmedversion/1gms/1gmfreqstem");
    bifreqchunk.push_back("/raid6/workspace/zichu/hadoopkeyset/stemmedversion/2gms/2gmfreqstemfilter");

    std::cout << "construction of giindexer finished " << std::endl;


    std::vector<std::string> bigramfreqvec;
    bigramfreqvec.push_back("/raid6/workspace/zichu/hadoopkeyset/stemmedversion/naivegffreqdataset/xaa");

    auto t1 = std::chrono::high_resolution_clock::now();
    textsim::MultiBigram *bigram = new textsim::MultiBigram(combineindexer);
    bigram->initbigram(bigramfreqvec);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "gfindexer construction takes " << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
              << " microseconds";

    auto t3 = std::chrono::high_resolution_clock::now();

    std::cout << bigram->get_freq("with") << std::endl;
    std::cout << bigram->get_freq("0000505") << std::endl;
    std::cout << bigram->get_freq("0000190") << std::endl;
    std::cout << bigram->get_freq("0000505") << std::endl;
    std::cout << bigram->get_freq("srid") << std::endl;
    std::cout << bigram->get_freq("srikara") << std::endl;
    std::cout << bigram->get_freq("srikaya") << std::endl;
    std::cout << bigram->get_freq("punma") << std::endl;
    std::cout << bigram->get_freq("puntazo") << std::endl;
    std::cout << bigram->get_freq("puntod") << std::endl;
    std::cout << bigram->get_freq("0 0") << std::endl;
    std::cout << bigram->get_freq("0 0000000000") << std::endl;
    std::cout << bigram->get_freq("0 000000000004") << std::endl;
    std::cout << bigram->get_freq("0 000000000007") << std::endl;
    std::cout << bigram->get_freq("0 00000000001") << std::endl;
    std::cout << bigram->get_freq("0 000000000011") << std::endl;
    std::cout << bigram->get_freq("0 000000000029") << std::endl;
    std::cout << bigram->get_freq("0 000000000042") << std::endl;
    std::cout << bigram->get_freq("0 000000000052") << std::endl;
    std::cout << bigram->get_freq("0 000000000058") << std::endl;


    auto t4 = std::chrono::high_resolution_clock::now();
    std::cout << "gfindexer retrieval 20 takes " << std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count()
              << " microseconds";
};

void test_aligned_gfindexer() {
     std::vector<std::string> keysetpathvec;

     keysetpathvec.push_back("/raid6/workspace/zichu/hadoopkeyset/stemmedversion/1gms/1gmfromallstem");

     std::string unimphpath = "/raid6/workspace/zichu/hadoopkeyset/stemmedversion/1gms/1gmfromallstem.mph";
     std::string bimphpath  = "/raid6/workspace/zichu/hadoopkeyset/stemmedversion/bigramindexoutpure0726.mph";
     std::string conmphpath = "/raid6/workspace/zichu/hadoopkeyset/stemmedversion/contextindexoutpure0726.mph";

     std::string stopwordfile = "/raid6/workspace/zichu/phsim/PhSim/resources/stopwords";
     std::cout<<"after stopwordfile"<<std::endl;

     std::cout<<"hadoop perfect fun finished "<<std::endl;

     std::string bigramspath = "/raid6/workspace/zichu/hadoopkeyset/stemmedversion/2gms/2gmstempure";
     std::string contextspath =  "/raid6/workspace/zichu/hadoopkeyset/stemmedversion/2gms/contextsstempure";

     textsim::CombineRangeIndexer *combineindexer = new textsim::CombineRangeIndexer(unimphpath,bimphpath);
     combineindexer->initcmphbi(bimphpath);
     combineindexer->initcmphcon(conmphpath);

     combineindexer->init(keysetpathvec);
     std::cout<<"init uni finish"<<std::endl;
     combineindexer->initbi(bigramspath);
     std::cout<<"init bi finish"<<std::endl;
     combineindexer->initcon(contextspath);
     std::cout<<"init con finish"<<std::endl;


     std::vector<std::string> unifreqchunk;
     std::vector<std::string> bifreqchunk;
     unifreqchunk.push_back("/raid6/workspace/zichu/hadoopkeyset/stemmedversion/1gms/1gmfreqstem");
     bifreqchunk.push_back("/raid6/workspace/zichu/hadoopkeyset/stemmedversion/2gms/2gmfreqstemfilter");

     textsim::CompressedmixGFIndexer *gfindexer = new textsim::CompressedmixGFIndexer(combineindexer);
     gfindexer->inituni(unifreqchunk);
     std::cout<<"gf uni finish"<<std::endl;
     gfindexer->initbi(bifreqchunk);
     std::cout<<"gf bi finish"<<std::endl;

     std::ifstream source;
     source.open("/raid6/workspace/zichu/hadoopkeyset/stemmedversion/1gms/1gmfreqstem",std::ios::in);

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

     source.open("/raid6/workspace/zichu/hadoopkeyset/stemmedversion/2gms/2gmfreqstemfilter",std::ios::in);
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

}

}

#endif //TEXTSIM_PROCESS_HPP
