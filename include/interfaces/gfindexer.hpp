#ifndef TEXTSIM_GFINDEXER_HPP
#define TEXTSIM_GFINDEXER_HPP
#include <string>

namespace textsim {

class GFindexer {
 public:
  // Get the frequence of the given bigram string in the ngram corpus.
  virtual unsigned long get_freq(std::string targetphrase) = 0;
};
}  // namespace phsim
#endif //TEXTSIM_GFINDEXER_HPP
