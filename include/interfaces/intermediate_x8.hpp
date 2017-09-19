#ifndef TEXTSIM_INTERMEDIATE_X8_HPP
#define TEXTSIM_INTERMEDIATE_X8_HPP

namespace textsim{
class intermediate_x8{
 public:
  virtual void encode_x8(std::vector <uint32_t> &input,
                          uint64_t &inputsize,
                          std::vector <uint8_t> &output,
                          uint64_t &intermediatesize) = 0;

  virtual void decode_x8(std::vector <uint8_t> &input,
                          uint64_t &inputsize,
                          std::vector <uint32_t> &output,
                          uint64_t &intermediatesize) = 0;
};

}

#endif //TEXTSIM_INTERMEDIATE_X8_HPP
