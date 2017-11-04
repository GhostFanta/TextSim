#ifndef TEXTSIM_LINEAR_HPP
#define TEXTSIM_LINEAR_HPP

#include "common.hpp"
#include "tool.hpp"
#include "interfaces/intersection.hpp"

namespace textsim{
class linearintersection:public textsim::intersection{
 public:
  virtual void intersect(std::vector<std::pair<uint32_t,uint64_t>> & posting1,
                         std::vector<std::pair<uint32_t,uint64_t>> & posting2,
                         std::vector<std::pair<uint32_t,uint64_t>> & result1,
                         std::vector<std::pair<uint32_t,uint64_t>> & result2) {
    size_t p_first = 0;
    size_t p_second = 0;

    if(posting1.size()<posting2.size()){
      while(p_first < posting1.size() && p_second < posting2.size()){
        if(posting1[p_first].first == posting2[p_second].first){
          result1.push_back(posting1[p_first]);
          result2.push_back(posting2[p_second]);
          ++p_first;
          ++p_second;
        }else if(posting1[p_first].first > posting2[p_second].first){
          ++p_second;
        }else{
          ++p_first;
        }
      }
    }else{
      while(p_first < posting1.size() && p_second < posting2.size()){
        if(posting1[p_first].first == posting2[p_second].first){
          result1.push_back(posting1[p_first]);
          result2.push_back(posting2[p_second]);
          ++p_first;
          ++p_second;
        }else if(posting1[p_first].first > posting2[p_second].first){
          ++p_second;
        }else{
          ++p_first;
        }
      }
    }
  };

  virtual void intersect(std::unordered_map<uint32_t,uint64_t> &posting1,
                         std::unordered_map<uint32_t,uint64_t> &posting2,
                         std::vector<std::pair<uint32_t,uint64_t>> &result1,
                         std::vector<std::pair<uint32_t,uint64_t>> &result2) {
    throw std::runtime_error("Linear detection does not support hashmap!");
  };
};
}

#endif //TEXTSIM_LINEAR_HPP
