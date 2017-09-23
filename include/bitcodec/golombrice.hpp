#ifndef TEXTSIM_GOLOMBRICE_HPP
#define TEXTSIM_GOLOMBRICE_HPP

#include "common.hpp"
#include "bitalignedcommon.hpp"

#include "interfaces/intermediate_x64.hpp"

namespace textsim{
class golombrice:public bitalignedcommon,public intermediate_x64{
 public:
  void encode_x64(std::vector <uint32_t> &input,
                  uint64_t &inputsize,
                  std::vector <uint64_t> &output,
                  uint64_t &intermediatesize) {

  };

  void decode_x64(std::vector<uint64_t> &input,
                  uint64_t &inputsize,
                  std::vector<uint32_t> &output,
                  uint64_t &intermediatesize){

  }
};
};
#endif //TEXTSIM_GOLOMBRICE_HPP
