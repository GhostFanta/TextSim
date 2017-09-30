#ifndef TEXTSIM_INVERTEDGCFOUR_HPP
#define TEXTSIM_INVERTEDGCFOUR_HPP

#include "common.hpp"
#include "tool.hpp"
#include "interfaces/gcindexer.hpp"
#include "interfaces/intermediate_x64.hpp"
#include "interfaces/intermediate_x32.hpp"
#include "interfaces/intermediate_x8.hpp"

namespace textsim {
class InvertedGCIndexerFour_x8:public GCIndexer{
 private:

 public:
  std::vector <uint64_t> get_contexts(std::string targetphrase) {

  };
};
};
#endif //TEXTSIM_INVERTEDGCFOUR_HPP
