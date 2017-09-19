#ifndef TEXTSIM_GIINDEXER_HPP
#define TEXTSIM_GIINDEXER_HPP
#include <string>
#include <cstdint>

namespace textsim {

class GIIndexer {

 public:

  // The identifier for non-existing unigram/bigram string.
  static const int NOT_EXIST = 0;

  // Get the unique identifier of the given unigram.
  virtual unsigned int get_unigram_id(std::string unigram) = 0;

  // Get the unique identifier of the given bigram.
  virtual unsigned int get_bigram_id(std::string bigram) = 0;
};

}  // namespace phsim
#endif //TEXTSIM_GIINDEXER_HPP
