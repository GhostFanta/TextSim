#ifndef TEXTSIM_VARIANTBYTE_HPP
#define TEXTSIM_VARIANTBYTE_HPP
#include "interfaces/intermediate_x8.hpp"
#include "common.hpp"
#include "tool.hpp"

namespace textsim{
// Use 0 to represent the end of available bytes.

    class vbyte: public intermediate_x8{
    public:

        template<uint32_t offset>
        inline uint8_t extract7bits(const uint32_t input) {
            return static_cast<uint8_t>((input >> (offset * 7)) & ((1 << 7) - 1));
        }

        // recover from byte with descriptor
        template<uint32_t offset>
        inline uint32_t recover32bits_x7(uint32_t &container, uint8_t memberbits) {
            uint8_t temp = memberbits << 1;
            container |= ((temp >> 1) << (7 * offset));
        }

        inline bool isnotend(uint8_t &input) {
            return input >> 7;
        }

    public:
        void encode_x8(std::vector<uint32_t> &input,size_t &originalsize,std::vector<uint8_t> &output,size_t &intermediatesize) {
            output.resize(input.size() * 4);
            size_t index_byte = 0;
            for (size_t i = 0; i < input.size();) {
                uint32_t val = input[i];
                if (val < (1U << 7)) {
                    output[index_byte] = extract7bits<0>(val);
                    ++index_byte;
                } else if (val < (1U << 14)) {
                    output[index_byte] = extract7bits<0>(val) | (1 << 7);
                    ++index_byte;
                    output[index_byte] = extract7bits<1>(val);
                    ++index_byte;
                } else if (val < (1U << 21)) {
                    output[index_byte] = extract7bits<0>(val) | (1 << 7);
                    ++index_byte;
                    output[index_byte] = extract7bits<1>(val) | (1 << 7);
                    ++index_byte;
                    output[index_byte] = extract7bits<2>(val);
                    ++index_byte;
                } else if (val < (1U << 28)) {
                    output[index_byte] = extract7bits<0>(val) | (1 << 7);
                    ++index_byte;
                    output[index_byte] = extract7bits<1>(val) | (1 << 7);
                    ++index_byte;
                    output[index_byte] = extract7bits<2>(val) | (1 << 7);
                    ++index_byte;
                    output[index_byte] = extract7bits<3>(val);
                    ++index_byte;
                } else {
                    output[index_byte] = extract7bits<0>(val) | (1 << 7);
                    ++index_byte;
                    output[index_byte] = extract7bits<1>(val) | (1 << 7);
                    ++index_byte;
                    output[index_byte] = extract7bits<2>(val) | (1 << 7);
                    ++index_byte;
                    output[index_byte] = extract7bits<3>(val) | (1 << 7);
                    ++index_byte;
                    output[index_byte] = extract7bits<4>(val);
                    ++index_byte;
                }
                ++i;
            };
            output.resize(index_byte);
        };

        void decode_x8(std::vector<uint8_t> &input,size_t &intermediatesize,std::vector<uint32_t> &output,size_t &expectedelenum) {
            size_t numofbytes = 0;
            uint8_t cache[5];
            for (size_t i = 0; i < input.size(); i++) {
                if (this->isnotend(input[i])) {
                    cache[numofbytes] = input[i];
                    ++numofbytes;
                } else {
                    cache[numofbytes] = input[i];
                    uint32_t mem = 0;
                    switch (numofbytes) {
                        case 0:
                            this->recover32bits_x7<0>(mem, cache[0]);
                            break;
                        case 1:
                            this->recover32bits_x7<0>(mem, cache[0]);
                            this->recover32bits_x7<1>(mem, cache[1]);
                            break;
                        case 2:
                            this->recover32bits_x7<0>(mem, cache[0]);
                            this->recover32bits_x7<1>(mem, cache[1]);
                            this->recover32bits_x7<2>(mem, cache[2]);
                            break;
                        case 3:
                            this->recover32bits_x7<0>(mem, cache[0]);
                            this->recover32bits_x7<1>(mem, cache[1]);
                            this->recover32bits_x7<2>(mem, cache[2]);
                            this->recover32bits_x7<3>(mem, cache[3]);
                            break;
                        case 4:
                            this->recover32bits_x7<0>(mem, cache[0]);
                            this->recover32bits_x7<1>(mem, cache[1]);
                            this->recover32bits_x7<2>(mem, cache[2]);
                            this->recover32bits_x7<3>(mem, cache[3]);
                            this->recover32bits_x7<4>(mem, cache[4]);
                            break;
                    }
                    numofbytes = 0;
                    output.push_back(mem);
                }
            }
        };
    };
};
#endif //TEXTSIM_VARIANTBYTE_HPP
