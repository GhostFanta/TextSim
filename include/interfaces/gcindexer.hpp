#ifndef TEXTSIM_GCINDEXER_HPP
#define TEXTSIM_GCINDEXER_HPP

#include "common.hpp"

namespace textsim {
class GCIndexer{
 public:
  // Get all the contexts of the given bigram string in the fourgram corpus.
  virtual void get_contexts(std::string targetphrase,
                       std::vector<std::pair<uint64_t,uint64_t>> &contextarray) = 0;
};

}  // namespace phsim
#endif //TEXTSIM_GCINDEXER_HPP
