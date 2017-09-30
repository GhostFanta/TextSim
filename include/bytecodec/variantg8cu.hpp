#ifndef TEXTSIM_VARIANTG8CU_HPP
#define TEXTSIM_VARIANTG8CU_HPP

#include "interfaces/intermediate_x8.hpp"
#include "bytealignedcommon.hpp"

namespace textsim{

class variantg8cu:public bytealignedcommon, public intermediate_x8{
 private:
  void write_descriptor(uint8_t unary, uint8_t &descriptor,
                        uint8_t &descriptorlen, std::vector<uint8_t> &data){

  };

  uint8_t read_descriptor(){

  };

 public:
  void encode_x8(std::vector <uint32_t> &input,
                 uint64_t &inputsize,
                 std::vector <uint8_t> &output,
                 uint64_t &intermediatesize) {
    uint8_t descriptor = 0;
    uint8_t descriptorspace = 8;
    for(size_t i = 0 ; i < input.size() ; ++i){
      size_t num_bytes = this->requiredbytes(input[i]);
      // Current descript not enough
      if(descriptorspace < num_bytes){

      }else{

      }
    };
  };

  void decode_x8(std::vector <uint8_t> &input,
                 uint64_t &inputsize,
                 std::vector <uint32_t> &output,
                 uint64_t &recoversize) {
    output.resize(recoversize);
    while(recoversize > 0){

    };
  };
};
}

#endif //TEXTSIM_VARIANTG8CU_HPP
