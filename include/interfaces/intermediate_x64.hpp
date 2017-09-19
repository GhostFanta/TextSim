#ifndef TEXTSIM_INTERMEDIATE_X64_HPP
#define TEXTSIM_INTERMEDIATE_X64_HPP

namespace textsim {
class intermediate_x64 {
 public:
  virtual void encode_x64(std::vector <uint32_t> &input,
                          uint64_t &inputsize,
                          std::vector <uint64_t> &output,
                          uint64_t &intermediatesize) = 0;

  virtual void decode_x64(std::vector <uint64_t> &input,
                          uint64_t &inputsize,
                          std::vector <uint32_t> &output,
                          uint64_t &intermediatesize) = 0;
};

}

#endif //TEXTSIM_INTERMEDIATE_X64_HPP
