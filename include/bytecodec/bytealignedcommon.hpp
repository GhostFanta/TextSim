#ifndef TEXTSIM_BYTEALIGNEDCOMMON_HPP
#define TEXTSIM_BYTEALIGNEDCOMMON_HPP

namespace textsim{
class bytealignedcommon{
 protected:
  // Taken from Lemire's lib
  // Return required byte of one integer
  inline uint32_t requiredbytes(uint32_t input) {
    return ((__builtin_clz(input | 255) ^ 31) >> 3) + 1;
  };

  // Taken bytes according to increasing byte address
  template<uint32_t offset>
  inline uint8_t extract8bits(const uint32_t input) {
    return static_cast<uint8_t>((input >> (offset * 8)) & ((1 << 8) - 1));
  }
};

}

#endif //TEXTSIM_BYTEALIGNEDCOMMON_HPP
