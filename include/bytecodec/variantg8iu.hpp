#ifndef TEXTSIM_VARIANTG8IU_HPP
#define TEXTSIM_VARIANTG8IU_HPP

#ifdef __GNUC__
#define PREDICT_FALSE(x) (__builtin_expect(x,0))
#else
#define PREDICE_FALSE(x) x
#endif

#include "common.h"
#include "interfaces/intermediate_x8.hpp"


namespace textsim{
// Write data first, then write descriptor
class variantg8iu : public textsim::intermediate_x8{

#define UNITWIDTH 8
#define TEMPBLOCKSIZE 8

  __m128i vecmask [256][2];
 private:
  // Because we only need unary to represent num of bits, so we need only set 0s.
  const uint8_t set_bit_look_up_table[8] = {0b11111110,0b11111101,0b11111011,0b11110111,
                                            0b11101111,0b11011111,0b10111111,0b01111111};
  /**
   *
   * @tparam num_bytes : how many bytes does candidate element require
   * @param descriptor : current descriptor
   * @param descriptoroffset : offset index inside the descriptor
   * @param tempblock : buffer for 8 bytes
   * @param data
   */
  template <uint8_t num_bytes>
  // return a flag to indicate whether we need to trigger flush
  // we need to do flush in this function
  inline void write_descriptor(uint8_t &descriptor,
                        uint8_t &descriptoroffset,
                        std::vector<uint8_t> &tempblock,
                        uint8_t &tempblockoffset,
                        std::vector<uint8_t> &data){
    if(num_bytes > (UNITWIDTH - descriptoroffset)) {
      data.push_back(descriptor);
      descriptor = 0xff;
      descriptoroffset = 0;
      for(int i = 0 ; i < tempblock.size() ; i++){
        data.push_back(tempblock[i]);
        tempblock[i] = 0;
        tempblockoffset = 0;
      }
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
          descriptoroffset += 2;
          descriptor &= set_bit_look_up_table[descriptoroffset++];

          break;
        case 4:
          descriptoroffset += 3;
          descriptor &= set_bit_look_up_table[descriptoroffset++];

          break;
      }
    };

  /**
  *
   * @param ele : element to be encoded
   * @param tempblock : buffer for one block(8 bytes)
   * @param tempblockoffset : offset index inside buffer
   * @param data : ecoded array of whole
   */
  inline void write_block(uint32_t &ele,std::vector<uint8_t> &tempblock,uint8_t &tempblockoffset, std::vector<uint8_t> &data){
    ASSERT(tempblock.size() == 8,"temp block must be a vector with capacity 16");
    std::vector<uint8_t> tempvec;
    auto numbyte = this->num_of_byte(ele);
    for (int i = 0 ; i < numbyte; ++i){
      uint8_t tempele = (ele >> (8 * i)) & (uint8_t)0xff;
      tempvec.push_back(tempele);
    }
    for(int i = tempvec.size() - 1 ; i >= 0 ; --i){
      tempblock[tempblockoffset] = (tempvec[i]);
      ++tempblockoffset;
    }
  }

 public:
  inline int num(uint8_t val){
    return 8 -__builtin_popcount(val);
  }

//  inline int len(const uint8_t &descriptor,uint8_t offset){
//    int prev = 0;
//    int cur = 0;
//    while(cur < 8){
//      while()
//    }
//  }
 private:

  inline void generate_descriptor(std::vector<uint8_t> &store,uint8_t cur, uint8_t offset){
    if(offset >= 8){
      store.push_back(cur);
      return;
    }
    cur &= ~(1 << (offset));
    generate_descriptor(store,cur,offset + 1);
    generate_descriptor(store,cur,offset + 2);
    generate_descriptor(store,cur,offset + 3);
    generate_descriptor(store,cur,offset + 4);
  }

  // Which means read from low bit to high bit, from right to left.
  inline bool readbytefromlowtohigh(const uint8_t &content,uint8_t offset){
    return (content & (1U << offset));
  }

  inline void construct_suffle_sequence(){
    std::vector<uint8_t> validdescriptors;
    this->generate_descriptor(validdescriptors,0xff,0);
    char mask[256][32];
    for(int i = 0 ; i < 256; i++){
      memset(mask[i],-1,32);
    }

    for(auto descriptor : validdescriptors){
      uint8_t prevoffset = 0;
      uint8_t curoffset = 0;
      uint8_t maskoffset = 0;
      for(int eleindex =  0; eleindex < this->num(descriptor) ; ++eleindex){

      }
    }
  }

  inline void flush(std::vector<uint8_t > &data,uint8_t &descriptor,uint8_t &descriptoroffset,std::vector<uint8_t> &tempblock, uint8_t &tempblockoffset){
    data.push_back(descriptor);
    descriptor = 0xff;
    descriptoroffset = 0;
    for(int i = 0 ; i < tempblock.size() ; i++){
      data.push_back(tempblock[i]);
      tempblock[i] = 0;
    }
    tempblockoffset = 0;
  }

 public:
  void encode_x8(std::vector <uint32_t> &input,
                         uint64_t &inputsize,
                         std::vector <uint8_t> &output,
                         uint64_t &intermediatesize) {
    uint8_t descriptor = 0xff;
    uint8_t descriptoroffset = 0;
    std::vector<uint8_t> tempblock(8);
    uint8_t tempoffset = 0;
    for(size_t i = 0 ; i < input.size() ;i++){
      auto bytenum = this->num_of_byte(input[i]);
      switch (bytenum){
        case 1:
          this->write_descriptor<1>(descriptor,descriptoroffset,tempblock,tempoffset,output);
          break;
        case 2:
          this->write_descriptor<2>(descriptor,descriptoroffset,tempblock,tempoffset,output);
          break;
        case 3:
          this->write_descriptor<3>(descriptor,descriptoroffset,tempblock,tempoffset,output);
          break;
        case 4:
          this->write_descriptor<4>(descriptor,descriptoroffset,tempblock,tempoffset,output);
          break;
      }
      this->write_block(input[i],tempblock,tempoffset,output);
    };
    intermediatesize = output.size();
    this->flush(output,descriptor,descriptoroffset,tempblock,tempoffset);
    std::cout<<std::endl;
  };

  void decode_x8(std::vector <uint8_t> &input,
                         uint64_t &inputsize,
                         std::vector <uint32_t> &output,
                         uint64_t &intermediatesize) {

  };
};
}
#endif //TEXTSIM_VARIANTG8IU_HPP
