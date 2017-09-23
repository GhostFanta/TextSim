#ifndef TEXTSIM_BIT_VECTOR_HPP
#define TEXTSIM_BIT_VECTOR_HPP

#include "common.hpp"
#include "tool.hpp"

namespace textsim{
class bit_vector_handler{

 #define WORDSIZE 64

 private:
  // First element of data records num_of_bits
  std::vector<uint64_t> &data;
  uint64_t _buffer;
  uint64_t _num_of_buffered_bits; // Should only record remaining bits not aligned to WORDSIZE

 public:

  bit_vector_handler(std::vector<uint64_t> &intermediate):data(intermediate){
    // Valid data container should at least have two eles.
    if(this->data.size() < 2){
      this->data.push_back(0);
      this->_buffer = 0;
      this->_num_of_buffered_bits = 0;
    }else{
      this->_num_of_buffered_bits = this->data[0] % WORDSIZE;
      this->_buffer = this->data.back();
      this->data.pop_back();
    }
  }

  ~bit_vector_handler(){
    if(this->_num_of_buffered_bits != 0){
      this->flush();
    }
    this->trim();
  };

  inline uint64_t read_bits(size_t len){
    ASSERT(len <= this->data[0],"cannot read bits more than storage");
    this->data[0] -= len;
    if(len > _num_of_buffered_bits){
      size_t res = 0;
      size_t num_lowbits = _num_of_buffered_bits;
      size_t num_highbits = len - num_lowbits;
      res |= (this->_buffer >> (WORDSIZE - this->_num_of_buffered_bits));
      this->_buffer = this->data.back();
      this->_num_of_buffered_bits = WORDSIZE - num_highbits;
      textsim::logger::show_uint64t_binary(res | ((((1 << num_highbits) - 1) & this->_buffer) << num_lowbits));
      return res | ((((1 << num_highbits) - 1) & this->_buffer) << num_lowbits);
    }else{
      this->_num_of_buffered_bits -= len;
      size_t mask = (1 << len) - 1;
      return (this->_buffer >> (WORDSIZE - this->_num_of_buffered_bits - len)) & mask;
    }
  }

  inline bool read_bit(){
    ASSERT(0 != this->data[0],"empty storage!");
    --this->data[0];              // Decrease all bits
    if(_num_of_buffered_bits == 0){
      this->_buffer = this->data.back();
      this->data.pop_back();
      --this->_num_of_buffered_bits;// Decrease buffered bits
      return this->_buffer & 1;
    }else{
      return this->_buffer & (1ul << --this->_num_of_buffered_bits);
    }
  }

  inline bool is_empty(){
    return this->data[0] == 0;
  }

  template <bool bit_content>
  inline void write_bit(){
    // bit size increment
    ++this->data[0];
    // If current buffer is full
    if(_num_of_buffered_bits == WORDSIZE){
      this->flush();
      this->_num_of_buffered_bits = 1;
      this->_buffer |= ((uint64_t)bit_content << (WORDSIZE - 1));
    }else{
      this->_buffer |= ((uint64_t)bit_content << (WORDSIZE - 1  - this->_num_of_buffered_bits));
      ++this->_num_of_buffered_bits;
    }
  }

  inline void write_bits(uint64_t val,size_t num_of_bits){
    // Cannot handle bit width larger than buffer size at one operation.
    ASSERT(num_of_bits < 64,"unable to handle data more than 64 bits at once");
    this->data[0] += num_of_bits;
    // If buffer will be full
    if(num_of_bits > WORDSIZE - this->_num_of_buffered_bits){
      size_t num_highbits = WORDSIZE - this->_num_of_buffered_bits;
      size_t num_lowbits = num_of_bits - num_highbits;
      this->_buffer |= val >> num_lowbits;
      this->flush();
      size_t mask = (1 << num_lowbits) - 1;
      this->_buffer |= ((val & mask)<<(WORDSIZE - num_lowbits));
      this->_num_of_buffered_bits += num_lowbits ;
    } else {
      this->_buffer |= val << (WORDSIZE - this->_num_of_buffered_bits - num_of_bits);
      this->_num_of_buffered_bits += num_of_bits;
    }
  };

 private:
  inline void flush(){
    this->data.push_back(this->_buffer);
    this->_buffer = 0ul;
    this->_num_of_buffered_bits = 0;
  }

  inline void read_buffer(){
    if(this->_num_of_buffered_bits != 0){
      this->_buffer = (this->data.back() >> (WORDSIZE - _num_of_buffered_bits));
      this->data.pop_back();
    }
  }

 public:
  inline void trim(){
    this->data.shrink_to_fit();
  }

  inline void clear(){
    this->data.resize(1);
    this->data[0] = 0;
    this->_num_of_buffered_bits = 0;
  }

 public:
  template <size_t offset>
  inline void look_data(){
    ASSERT(offset < this->data.size(),"required offset larger than data size");
    std::cout<<"now you have "<<this->data[0]<<" \tbits,\t"<<this->_num_of_buffered_bits<<" buffered bits"<<std::endl;
    std::bitset<64> demo(this->data[offset]);
    std::cout<<"mem["<<offset<<"]:\t"<<demo<<std::endl;
  }

  inline void look_buf(){
    std::bitset<64> demo(this->_buffer);
    std::cout<<"buf:\t"<<demo<<std::endl;
  }

  inline void check_buffer(size_t expectedbuffer){
    ASSERT(expectedbuffer == this->_buffer,"buffer not equal to expectation");
  }
};
};
#endif //TEXTSIM_BIT_VECTOR_HPP