#ifndef TEXTSIM_ELIASOMEGA_HPP
#define TEXTSIM_ELIASOMEGA_HPP

#include "common.hpp"
#include "bitalignedcommon.hpp"

#include "interfaces/intermediate_x64.hpp"
namespace textsim{
class elias_omega:public bitalignedcommon,public intermediate_x64{
 public:
  void encode_x64(std::vector <uint32_t> &input,
                          uint64_t &inputsize,
                          std::vector <uint64_t> &output,
                          uint64_t &intermediatesize) {
    bit_vector_handler handler(output);
    for(size_t i = 0 ;i < input.size(); ++i){
      handler.write_bit<0>();
      if(input[i] == 1){
        continue;
      }
      size_t data = input[i];
      size_t num_bits = _num_of_binary_bits(data);
      while(num_bits > 1){
        handler.write_bits_reverse(data, num_bits);
        data = num_bits - 1;
        num_bits = _num_of_binary_bits(data);
      }
    }
  };

  void decode_x64(std::vector<uint64_t> &input,
                  uint64_t &inputsize,
                  std::vector<uint32_t> &output,
                  uint64_t &recoversize){
    bit_vector_handler handler(input);
    output.resize(recoversize);
    while(!handler.is_empty()){
      size_t N = 1;
      while(handler.read_bit() == 1){
        N = (1 << N) | handler.read_bits_reverse(N);
      }
      output[recoversize - 1] = N;
      --recoversize;
    };
  }
};
};

#endif //TEXTSIM_ELIASOMEGA_HPP
