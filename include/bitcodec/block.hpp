#ifndef TEXTSIM_BLOCK_HPP
#define TEXTSIM_BLOCK_HPP

#include "common.hpp"
#include "interfaces/intermediate_x64.hpp"
#include "bitalignedcommon.hpp"
#include "util/bit_vector.hpp"

namespace textsim{
template <size_t blocksize>
class block:public intermediate_x64,public bitalignedcommon{
 public:
  /**
   *
   * @param input
   * @param inputsize
   * @param output
   * @param intermediatesize
   */
  void encode_x64(std::vector <uint32_t> &input,
                          uint64_t &inputsize,
                          std::vector <uint64_t> &output,
                          uint64_t &intermediatesize) {
    bit_vector_handler handler(output);
    for(auto ele : input){
      size_t num_bits = _num_of_binary_bits(ele);
      size_t num_blocks = (num_bits + 1) / blocksize;
      size_t mask = (1 << blocksize) - 1;
      for(size_t i = 0 ;i < num_blocks ;++i){
        handler.write_bits(mask & ele, blocksize);
        ele >>= blocksize;
      };
      handler.write_bit<1>();
      while(num_blocks-- > 1){
        handler.write_bit<0>();
      }
    }
  };


  /**
   * recover size should be exact the same with original size
   * @param input
   * @param inputsize
   * @param output
   * @param intermediatesize
   */
  void decode_x64(std::vector <uint64_t> &input,
                          uint64_t &inputsize,
                          std::vector <uint32_t> &output,
                          uint64_t &recoversize) {
    bit_vector_handler handler(input);
    output.resize(recoversize);
    while(!handler.is_empty()){
      size_t num_blocks = 1;
      while(!handler.read_bit()){
        ++num_blocks;
      }
      uint64_t member = 0;
      while(num_blocks > 0){
        ASSERT(num_blocks > 0,"numblock == 0");
        size_t chunk = handler.read_bits(blocksize);
        member |= (chunk << (blocksize * (num_blocks - 1)));
        --num_blocks;
      };
      output[recoversize - 1] = member;
      --recoversize;
    };
  };
};
};
#endif //TEXTSIM_BLOCK_HPP