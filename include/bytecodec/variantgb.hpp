#ifndef TEXTSIM_VARIANTGB_HPP
#define TEXTSIM_VARIANTGB_HPP
#include "common.hpp"
#include "interfaces/intermediate_x8.hpp"

namespace textsim{
    class variantgb: public intermediate_x8{
    public:
        template<uint32_t numofbytes>
        inline void setdescriptor(uint8_t &descriptor, size_t offset) {
            switch (numofbytes) {
                case 1:
                    descriptor |= ((0b00) << (offset * 2));
                    break;
                case 2:
                    descriptor |= ((0b01) << (offset * 2));
                    break;
                case 3:
                    descriptor |= ((0b10) << (offset * 2));
                    break;
                case 4:
                    descriptor |= ((0b11) << (offset * 2));
                    break;
            }
        }

        template<uint32_t offset>
        inline uint8_t readdescriptor(uint8_t &descriptor) {
            return (descriptor & (0b11 << (offset * 2))) >> (offset * 2);
        }

        template<uint32_t offset>
        inline void recover32bits_x8(uint32_t &container, uint8_t component) {
            container |= ((component) << (8 * offset));
        }

    public:
        void encode_x8(std::vector<uint32_t> &input, size_t &originalsize,std::vector<uint8_t> &output,size_t &intermediatesize) {
            size_t index_byte = 0;
            size_t originalsize = input.size();
            output.resize(originalsize * 4 + originalsize / 4 + 1);
            uint8_t descriptor = 0;
            uint8_t descriptorindex = 0;
            for (size_t i = 0; i < input.size(); i++) {
                size_t numofbytes = this->requiredbytes(input[i]);
                size_t whoisdescriptor;
                // When the second member is 2^n, a % b equals to a & (b - 1)
                if ((i & 3) == 0) {
                    whoisdescriptor = index_byte++;
                    descriptorindex = 0;
                }
                uint32_t val = input[i];

                switch (numofbytes) {
                    case 1:
                        this->setdescriptor<1>(output[whoisdescriptor], descriptorindex);
                        output[index_byte] = extract8bits<0>(val);
                        ++index_byte;

                        ++descriptorindex;
                        break;
                    case 2:
                        this->setdescriptor<2>(output[whoisdescriptor], descriptorindex);
                        output[index_byte] = extract8bits<0>(val);
                        ++index_byte;
                        output[index_byte] = extract8bits<1>(val);
                        ++index_byte;

                        ++descriptorindex;
                        break;
                    case 3:
                        this->setdescriptor<3>(output[whoisdescriptor], descriptorindex);
                        output[index_byte] = extract8bits<0>(val);
                        ++index_byte;
                        output[index_byte] = extract8bits<1>(val);
                        ++index_byte;
                        output[index_byte] = extract8bits<2>(val);
                        ++index_byte;

                        ++descriptorindex;
                        break;
                    case 4:
                        this->setdescriptor<4>(output[whoisdescriptor], descriptorindex);
                        output[index_byte] = extract8bits<0>(val);
                        ++index_byte;
                        output[index_byte] = extract8bits<1>(val);
                        ++index_byte;
                        output[index_byte] = extract8bits<2>(val);
                        ++index_byte;
                        output[index_byte] = extract8bits<3>(val);
                        ++index_byte;

                        ++descriptorindex;
                        break;
                };
            }
            output.resize(index_byte);
        }

        void decode_x8(std::vector<uint8_t> &input,size_t &intermediatesize,std::vector<uint32_t> &output, size_t &expectedelenum) {
            size_t eleindex = 0;
            uint8_t descriptor;

            output.resize(expectedelenum);

            for (size_t index = 0; index < input.size();) {

                // Read in one descriptor
                descriptor = input[index];
                ++index;

                // Store the size of next 4 elements(possible empty size);
                uint8_t compsize[4];
                compsize[0] = this->readdescriptor<0>(descriptor) + 1;
                compsize[1] = this->readdescriptor<1>(descriptor) + 1;
                compsize[2] = this->readdescriptor<2>(descriptor) + 1;
                compsize[3] = this->readdescriptor<3>(descriptor) + 1;

                // Cache of next 4 elements
                uint32_t comps[4] = {0, 0, 0, 0};

                // Decode next 4 elements
                for (size_t innerindex = 0; innerindex < 4; ++innerindex) {
                    switch (compsize[innerindex]) {
                        case 1:
                            this->recover32bits_x8<0>(comps[innerindex], input[index]);
                            ++index;

                            output[eleindex] = comps[innerindex];
                            ++eleindex;
                            break;
                        case 2:
                            this->recover32bits_x8<0>(comps[innerindex], input[index]);
                            ++index;
                            this->recover32bits_x8<1>(comps[innerindex], input[index]);
                            ++index;

                            output[eleindex] = comps[innerindex];
                            ++eleindex;
                            break;
                        case 3:
                            this->recover32bits_x8<0>(comps[innerindex], input[index]);
                            ++index;
                            this->recover32bits_x8<1>(comps[innerindex], input[index]);
                            ++index;
                            this->recover32bits_x8<2>(comps[innerindex], input[index]);
                            ++index;

                            output[eleindex] = comps[innerindex];
                            ++eleindex;
                            break;
                        case 4:
                            this->recover32bits_x8<0>(comps[innerindex], input[index]);
                            ++index;
                            this->recover32bits_x8<1>(comps[innerindex], input[index]);
                            ++index;
                            this->recover32bits_x8<2>(comps[innerindex], input[index]);
                            ++index;
                            this->recover32bits_x8<3>(comps[innerindex], input[index]);
                            ++index;

                            output[eleindex] = comps[innerindex];
                            ++eleindex;
                            break;
                    }
                    if (eleindex > expectedelenum) {
                        break;
                    }
                }
            }
        }
    };
};
#endif //TEXTSIM_VARIANTGB_HPP
