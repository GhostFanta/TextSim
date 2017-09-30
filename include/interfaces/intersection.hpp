#ifndef TEXTSIM_INTERSECTION_HPP
#define TEXTSIM_INTERSECTION_HPP
#include "common.hpp"
#include "tool.hpp"

namespace textsim{
class intersection{
 public:
  virtual void intersect(std::vector<std::pair<uint32_t,uint64_t>> & posting1,
                         std::vector<std::pair<uint32_t,uint64_t>> & posting2,
                         std::vector<std::pair<uint32_t,uint64_t>> & result1,
                         std::vector<std::pair<uint32_t,uint64_t>> & result2) = 0;
};
}
#endif //TEXTSIM_INTERSECTION_HPP
