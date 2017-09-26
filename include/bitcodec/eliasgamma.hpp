#ifndef TEXTSIM_ELIASGAMMA_HPP
#define TEXTSIM_ELIASGAMMA_HPP

#include "common.hpp"
#include "bitalignedcommon.hpp"
#include "interfaces/intermediate_x64.hpp"
#include "util/bit_vector.hpp"

namespace textsim{
/**
 * Prefix are stored after binary for convience, because bits are read backwards.
 * Use hard code for speed purposes.
 */
class elias_gamma: public bitalignedcommon,public intermediate_x64{
 public:
  /**
   * Elias Gamma Encode, inputsize is not necessary
   * @param input: data for compression
   * @param inputsize: size of data for compression, not necessary
   * @param output: intermediate data in bytes
   * @param intermediatesize: size of intermediate data, not necessary
   */
  void encode_x64(std::vector <uint32_t> &input,
                          uint64_t &inputsize,
                          std::vector <uint64_t> &output,
                          uint64_t &intermediatesize) {
    {
      bit_vector_handler handler(output);
      for (size_t i = 0; i < input.size(); ++i) {
        size_t num_bits = _num_of_binary_bits((uint64_t) input[i]);
        if (num_bits == 1) {
          handler.write_bit<1>();
//          handler.look_buf();
          continue;
        }
        size_t mask = (1 << (num_bits - 1)) - 1;
        handler.write_bits(input[i] & mask, num_bits - 1);
        handler.write_bit<1>();
        handler.write_bits(0, num_bits - 1);
      }
//      handler.look_buf();
    }
//    textsim::logger::show_uint64_array_binary(output,"");
  };

  /**
   * Elias Gamma Decode requires identification of recovered size for acceleration.
   * @param input: intermediate data in bytes
   * @param :
   * @param: container for recovered data.
   * @param: size of recovered size, necessary for acceleration.Do not use original value for output size.
   * */
  void decode_x64(std::vector<uint64_t> &input,
                  uint64_t &inputsize,
                  std::vector<uint32_t> &output,
                  uint64_t &recoversize){
    {
      bit_vector_handler handler(input);
      output.resize(recoversize);
      size_t numele = 0;
      while (!handler.is_empty()) {
        ASSERT(recoversize >= 0,"recoversize less than 0");
        size_t num_bits = 0;
        while (!handler.read_bit()) {
          ++num_bits;
        }
        if (!num_bits) {
          output[recoversize - 1] = 1;
        } else {
          size_t res = handler.read_bits(num_bits);
          output[recoversize - 1] = ((1UL << num_bits) | res);
        }
        --recoversize;
        handler.look_buf();
      };
      for(auto i : output){
        std::cout<<i<<"\t";
      }
      std::cout<<std::endl;
    }
  };
};
};
#endif //TEXTSIM_ELIASGAMMA_HPP