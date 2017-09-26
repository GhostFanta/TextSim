#ifndef TEXTSIM_ELIASDELTA_HPP
#define TEXTSIM_ELIASDELTA_HPP

#include "common.hpp"
#include "bitalignedcommon.hpp"
#include "interfaces/intermediate_x64.hpp"
#include "util/bit_vector.hpp"

namespace textsim{
class elias_delta:public bitalignedcommon,public intermediate_x64{
 public:
  void encode_x64(std::vector <uint32_t> &input,
                          uint64_t &inputsize,
                          std::vector <uint64_t> &output,
                          uint64_t &intermediatesize) {
    bit_vector_handler handler(output);
    for(size_t i = 0 ; i < input.size(); ++i){
      if(input[i] == 1){
        handler.write_bit<1>();
        continue;
      }
      size_t N = _log_2_floor(input[i]);
      size_t N_plusone_num = _num_of_binary_bits(N + 1);
      size_t lowbits = ((1 << N) - 1) & input[i];
      handler.write_bits(lowbits,N);
      handler.write_bits_reverse(N + 1,N_plusone_num);
      handler.write_bits(0,N_plusone_num - 1);
    }
  };

  void decode_x64(std::vector<uint64_t> &input,
                  uint64_t &inputsize,
                  std::vector<uint32_t> &output,
                  uint64_t &recoversize){
    bit_vector_handler handler(input);
    output.resize(recoversize);
    while(!handler.is_empty()){
      size_t prefix = 0;
      while(!handler.read_bit()){
        ++prefix;
      }
      if(prefix == 0){
        output[recoversize - 1] = 1;
        --recoversize;
        continue;
      }
      size_t high_indicator = ((1 << prefix) | handler.read_bits_reverse(prefix)) - 1;
      size_t low_bits = handler.read_bits(high_indicator);
      output[recoversize - 1] = (1 << high_indicator)  + low_bits;
      --recoversize;
    };
  };

};
}

#endif //TEXTSIM_ELIASDELTA_HPP