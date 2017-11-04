#ifndef TEXTSIM_GALLOPING_HPP
#define TEXTSIM_GALLOPING_HPP
#include "common.hpp"
#include "tool.hpp"
#include "interfaces/intersection.hpp"

namespace textsim{
class gallop:public intersection{
 private:
  uint64_t binary_search(std::vector<std::pair<uint32_t,uint64_t>> &input, size_t left, size_t right, uint32_t val){
    size_t mid;
    while(left <= right) {
      mid = left + (right - left) / 2;
      if (input[mid].first < val) {
//        std::cout<<"left = mid + 1"<<std::endl;
        left = mid + 1;
      } else if (input[mid].first > val) {
//        std::cout<<"right = mid - 1"<<std::endl;
        right = mid - 1;
      } else {
//        std::cout<<"get pos\t"<<mid<<std::endl;
        return mid;
      };
    }
    return std::numeric_limits<uint64_t>::max();
  }

  uint64_t gallop_search(std::vector<std::pair<uint32_t,uint64_t>> posting,
                        uint32_t val){
    if(posting.size() == 0){
      return std::numeric_limits<uint64_t>::max();
    };

    size_t right = 1;
    while(right < posting.size() && posting[right].first < val){
      right *= 2;
    }

    size_t rightend = right > posting.size() ? posting.size() : right;
    return this->binary_search(posting,right / 2,rightend,val);
  };

 public:
  void intersect(std::vector<std::pair<uint32_t,uint64_t>> & posting1,
                         std::vector<std::pair<uint32_t,uint64_t>> & posting2,
                         std::vector<std::pair<uint32_t,uint64_t>> & result1,
                         std::vector<std::pair<uint32_t,uint64_t>> & result2){
    if(posting1.size() > posting2.size()){
      for(auto i : posting2){
        auto index = this->gallop_search(posting1,i.first);
        if(index != std::numeric_limits<uint64_t>::max()){
          result1.push_back(posting1[index]);
          result2.push_back(i);
        }
      };
    }else{
      for(auto i : posting1){
        auto index = this->gallop_search(posting2,i.first);
        if(index != std::numeric_limits<uint64_t>::max()){
          result1.push_back(i);
          result2.push_back(posting2[index]);
        };
      };
    };
  };
};

};

#endif //TEXTSIM_GALLOPING_HPP
