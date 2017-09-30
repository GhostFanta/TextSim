#ifndef TEXTSIM_WORDWRITER_HPP
#define TEXTSIM_WORDWRITER_HPP

#include "common.hpp"
#include "tool.hpp"

namespace textsim{
class word64writer{
 private:
  uint64_t &data;
 public:
  void write_bit(){

  };

  void write_bits(){

  };

  void read_bit(){

  };

};

class word8writer{
 private:
  const static CAP = 8;
  uint8_t &data;
  uint8_t &dataspace;
 public:
  word8writer(uint8_t& input, uint8_t &dataspace):data(input),dataspace(dataspace){};

  template <bool val>
  void write_bit(){

  };


  void read_bit(){

  }
};

class word32writer{

};

}

#endif //TEXTSIM_WORDWRITER_HPP
