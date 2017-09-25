#ifndef TEXTSIM_BITALIGNEDCOMMON_HPP
#define TEXTSIM_BITALIGNEDCOMMON_HPP

#include "common.hpp"

namespace textsim{
class bitalignedcommon{
  const static uint64_t WORDSIZE = 64;
 protected:
  size_t _num_of_binary_bits(uint64_t input){
    return WORDSIZE - __builtin_clzl(input);
  }
};

};
#endif //TEXTSIM_BITALIGNEDCOMMON_HPP
