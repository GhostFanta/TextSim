#ifndef TEXTSIM_SIMPLE9_HPP
#define TEXTSIM_SIMPLE9_HPP

#include "common.hpp"
#include "simple9.h"
#include "interfaces/intermediate_x32.hpp"

namespace textsim{
class simple9codec : public intermediate_x32{
public:
 void encode_x32(std::vector<uint32_t> &input, size_t &originalsize, std::vector<uint32_t> &intermediate,size_t &intermediatesize){
     FastPForLib::Simple9<false> codec;
     originalsize = input.size();
     codec.encodeArray(input.data(), input.size(),intermediate.data(),intermediatesize);
 }

 void decode_x32(std::vector<uint32_t> &intermediate, size_t &intermediatesize, std::vector<uint32_t> &recovereddata,size_t &recoveredsize){
     FastPForLib::Simple9<false> codec;
     codec.decodeArray(intermediate.data(),intermediatesize,recovereddata.data(),recoveredsize);
 }
};
};

#endif //TEXTSIM_SIMPLE9_HPP
