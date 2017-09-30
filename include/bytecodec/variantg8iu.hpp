#ifndef TEXTSIM_VARIANTG8IU_HPP
#define TEXTSIM_VARIANTG8IU_HPP

#include "common.h"
#include "interfaces/intermediate_x8.hpp"

namespace textsim{
// Write data first, then write descriptor
class variantg8iu:public intermediate_x8{
# define UNITWIDTH 8
 private:
  const uint8_t set_bit_look_up_table[8] = {0b11111110,0b11111101,0b11111011,0b11110111,
                                            0b11101111,0b11011111,0b10111111,0b01111111};
  template <uint8_t num_bytes>
  void write_descriptor(uint8_t &descriptor, uint8_t &descriptoroffset, std::vector<uint8_t> &data){
    if(num_bytes > (UNITWIDTH - descriptoroffset)) {
      data.push_back(descriptor);
      descriptor = 0xff;
      descriptoroffset = 0;
    }
      switch (num_bytes){
        case 1:
          descriptor &= set_bit_look_up_table[descriptoroffset++];
          break;
        case 2:
          ++descriptoroffset;
          descriptor &= set_bit_look_up_table[descriptoroffset++];

          break;
        case 3:
          descriptoroffset+=2;
          descriptor &= set_bit_look_up_table[descriptoroffset++];
          descriptoroffset += 2;

          break;
        case 4:
          descriptoroffset+=3;
          descriptor &= set_bit_look_up_table[descriptoroffset++];

          break;
      }
    };

  uint8_t read_descriptor(uint8_t &descriptor, uint8_t &descriptoroffset, std::vector<uint8_t> &data){

  };


 public:
  void encode_x8(std::vector <uint32_t> &input,
                         uint64_t &inputsize,
                         std::vector <uint8_t> &output,
                         uint64_t &intermediatesize) {
    for(size_t i = 0 ; i < input.size() ;i++){

    };
  };

  void decode_x8(std::vector <uint8_t> &input,
                         uint64_t &inputsize,
                         std::vector <uint32_t> &output,
                         uint64_t &intermediatesize) {

  };
};
}
#endif //TEXTSIM_VARIANTG8IU_HPP
