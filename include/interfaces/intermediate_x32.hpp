#ifndef TEXTSIM_INTERMEDIATE_X32_HPP
#define TEXTSIM_INTERMEDIATE_X32_HPP

namespace textsim {
class intermediate_x32 {
 public:
  virtual void encode_x32(std::vector <uint32_t> &input,
                          uint64_t &inputsize,
                          std::vector <uint32_t> &output,
                          uint64_t &intermediatesize) = 0;

  virtual void decode_x32(std::vector <uint32_t> &input,
                          uint64_t &inputsize,
                          std::vector <uint32_t> &output,
                          uint64_t &intermediatesize) = 0;
};
}

#endif //TEXTSIM_INTERMEDIATE_X32_HPP
