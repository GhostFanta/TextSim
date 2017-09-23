#ifndef TEXTSIM_VARIANTG8CU_HPP
#define TEXTSIM_VARIANTG8CU_HPP

#include "interfaces/intermediate_x8.hpp"
namespace textsim{

class variantg8cu:public intermediate_x8{
 public:
  void encode_x8(std::vector <uint32_t> &input,
                 uint64_t &inputsize,
                 std::vector <uint8_t> &output,
                 uint64_t &intermediatesize) {

  };

  void decode_x8(std::vector <uint8_t> &input,
                 uint64_t &inputsize,
                 std::vector <uint32_t> &output,
                 uint64_t &intermediatesize) {

  };
};
}

#endif //TEXTSIM_VARIANTG8CU_HPP
