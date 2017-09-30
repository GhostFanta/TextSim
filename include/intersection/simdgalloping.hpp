#ifndef TEXTSIM_SIMDGALLOPING_HPP
#define TEXTSIM_SIMDGALLOPING_HPP
#include "common.hpp"
#include "tool.hpp"
#include "interfaces/intersection.hpp"

namespace textsim{
class simdgalloping:public intersection{
 public:
    void intersect(std::vector<std::pair<uint32_t,uint64_t>> & posting1,
                         std::vector<std::pair<uint32_t,uint64_t>> & posting2,
                         std::vector<std::pair<uint32_t,uint64_t>> & result1,
                         std::vector<std::pair<uint32_t,uint64_t>> & result2) {

    };
};
};
#endif //TEXTSIM_SIMDGALLOPING_HPP
