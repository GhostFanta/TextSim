#ifndef TEXTSIM_GOLOMBRICE_HPP
#define TEXTSIM_GOLOMBRICE_HPP

#include "common.hpp"
#include "bitalignedcommon.hpp"

#include "interfaces/intermediate_x64.hpp"

namespace textsim{
template <size_t b, size_t v>
class golomb_rice:public bitalignedcommon,public intermediate_x64{
 public:
  void encode_x64(std::vector <uint32_t> &input,
                  uint64_t &inputsize,
                  std::vector <uint64_t> &output,
                  uint64_t &intermediatesize) {
    {
      bit_vector_handler handler(output);

    }
  };

  void decode_x64(std::vector<uint64_t> &input,
                  uint64_t &inputsize,
                  std::vector<uint32_t> &output,
                  uint64_t &recoversize){
    {
      bit_vector_handler handler(output);
      output.resize(recoversize);
    }
  }
};
};
#endif //TEXTSIM_GOLOMBRICE_HPP