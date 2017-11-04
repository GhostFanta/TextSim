#ifndef TEXTSIM_SIMDGALLOPING_HPP
#define TEXTSIM_SIMDGALLOPING_HPP
#include "common.hpp"
#include "tool.hpp"
#include "interfaces/intersection.hpp"

namespace textsim{
#ifdef __GNUC__
#define COMPILER_LIKELY(x) __builtin_expect((x), 1)
#define COMPILER_RARELY(x) __builtin_expect((x), 0)
#else
#define COMPILER_LIKELY(x) x
#define COMPILER_RARELY(x) x
#endif
class simdgalloping:public intersection{

public:
const static size_t *T;

public:
    uint64_t binary_search(std::vector<std::pair<uint32_t,uint64_t>> &input,
                           size_t left,
                           size_t right,
                           uint32_t val){
        size_t mid;
        while(left <= right) {
            mid = left + (right - left) / 2;
            if (input[mid].first < val) {
                left = mid + 1;
            } else if (input[mid].first > val) {
                right = mid - 1;
            } else {
                return mid;
            };
        }
        return std::numeric_limits<uint64_t>::max();
    }

    void scalar_intersection(std::vector<std::pair<uint32_t,uint64_t>> &shortposting,
                            std::vector<std::pair<uint32_t,uint64_t>> &longposting,
                            size_t shortstarter,
                            size_t longstarter,
                            std::vector<std::pair<uint32_t,uint64_t>> &resultofshort,
                            std::vector<std::pair<uint32_t,uint64_t>> &resultoflong){
//        std::cout<<"-----------------------"<<std::endl;
//        std::cout<<"enter scalr tail"<<std::endl;
//        std::cout<<"short starter "<<shortstarter<<" longstarter "<<longstarter<<std::endl;
        for(; shortstarter < shortposting.size() && longstarter < longposting.size();){
//            std::cout<<"short index\t"<<shortstarter<<"\tlong index\t"<<longstarter<<std::endl;
            if(shortposting[shortstarter].first == longposting[longstarter].first){
//                std::cout<<"scalar match\t"<<shortstarter<<std::endl;
                resultofshort.push_back(shortposting[shortstarter]);
                resultoflong.push_back(longposting[longstarter]);
                ++shortstarter;
                ++longstarter;
            }else if(shortposting[shortstarter].first > longposting[longstarter].first){
                ++longstarter;
            }else{
                ++shortstarter;
            }

        }
    }

    // refer from Lemire
        void V1(std::vector<std::pair<uint32_t,uint64_t>> &shortposting,
                std::vector<std::pair<uint32_t,uint64_t>> &longposting,
                std::vector<std::pair<uint32_t,uint64_t>> &resultofshort,
                std::vector<std::pair<uint32_t,uint64_t>> &resultoflong){

            size_t shortindex = 0;
            size_t longindex = 0;

            if(longposting.size() - longindex < simdgalloping::T[0]){
                this->scalar_intersection(shortposting, longposting, shortindex, longindex, resultofshort, resultoflong);
                return;
            }

            std::vector<uint32_t> shortkey;
            std::vector<uint32_t> longkey;

            shortkey.resize(shortposting.size());
            longkey.resize(longposting.size());

            // assign keys for intersection
            for(size_t i = 0 ; i < shortposting.size() ; ++i){
                shortkey[i] = shortposting[i].first;
            }

            for(size_t i = 0 ; i < longposting.size() ; ++i){
                longkey[i] = longposting[i].first;
            }

            uint32_t *firstposting = shortkey.data();
            uint32_t *secondposting = longkey.data();

            // loop elements in short seq
            for(; shortindex < shortposting.size() && longindex + simdgalloping::T[0] < longposting.size(); ++shortindex){

                // repeat element for comparison
                __m128i R = _mm_set1_epi32(*(firstposting + shortindex));

                while(longkey[longindex - 1 + simdgalloping::T[0]] < shortkey[shortindex]){
                    longindex += simdgalloping::T[0];
                    if(longindex + simdgalloping::T[0] > longkey.size()){
                        goto SCALAR;
                    }
                }

                __m128i F = _mm_loadu_si128((__m128i *) (secondposting + longindex));
                __m128i G = _mm_loadu_si128((__m128i *) (secondposting + longindex + 4));

                __m128i J0 = _mm_cmpeq_epi32(F, R);
                __m128i J1 = _mm_cmpeq_epi32(G, R);

                __m128i judge = _mm_or_si128(J0,J1);
                if(_mm_movemask_epi8(judge)!=0){
                    resultofshort.push_back(shortposting[shortindex]);
                    resultoflong.push_back(longposting[this->binary_search(longposting,longindex,longindex + simdgalloping::T[0],shortposting[shortindex].first)]);
                }
            }
        SCALAR:
            this->scalar_intersection(shortposting, longposting, shortindex, longindex, resultofshort, resultoflong);
        };

        void V3(std::vector<std::pair<uint32_t,uint64_t>> &shortposting,
                std::vector<std::pair<uint32_t,uint64_t>> &longposting,
                std::vector<std::pair<uint32_t,uint64_t>> &resultofshort,
                std::vector<std::pair<uint32_t,uint64_t>> &resultoflong){

            size_t shortindex = 0;
            size_t longindex = 0;


//            std::cout<<longposting.size()<<std::endl;
            if(longposting.size() < simdgalloping::T[3]){
                this->scalar_intersection(shortposting, longposting, shortindex, longindex, resultofshort, resultoflong);
                return;
            }

            std::vector<uint32_t> shortkey;
            std::vector<uint32_t> longkey;

            shortkey.resize(shortposting.size());
            longkey.resize(longposting.size());

            // assign keys for intersection
            for(size_t i = 0 ; i < shortposting.size() ; ++i){
                shortkey[i] = shortposting[i].first;
            }

            for(size_t i = 0 ; i < longposting.size() ; ++i){
                longkey[i] = longposting[i].first;
            }

            uint32_t *firstposting = shortkey.data();
            uint32_t *secondposting = longkey.data();

            for(;shortindex < shortposting.size(); ++shortindex){

                while(longkey[longindex - 1 + simdgalloping::T[3]] < shortkey[shortindex]){
                    longindex += simdgalloping::T[3];
                    if(longindex + simdgalloping::T[3] > longkey.size()){
                        goto SCALAR;
                    }
                }

                if(longposting[longindex - 1 + simdgalloping::T[1]] >= shortposting[shortindex]){
                    if(longposting[longindex - 1 + simdgalloping::T[0]] >= shortposting[shortindex]){
                        __m128i R = _mm_set1_epi32(*(firstposting + shortindex));
                        __m128i F =  _mm_loadu_si128((__m128i*)(secondposting + longindex));
                        __m128i G =  _mm_loadu_si128((__m128i*)(secondposting + longindex + 4));
                        __m128i J0 = _mm_cmpeq_epi32(F,R);
                        __m128i J1 = _mm_cmpeq_epi32(G,R);
                        __m128i J = _mm_set1_epi32(0);
                        if(_mm_movemask_epi8(_mm_or_si128(J,J0))!=0){
                            resultofshort.push_back(shortposting[shortindex]);
                            resultoflong.push_back(longposting[this->binary_search(longposting,longindex,longindex + 4 - 1,shortkey[shortindex])]);
                        }else if(_mm_movemask_epi8(_mm_or_si128(J,J1))!=0){
                            resultofshort.push_back(shortposting[shortindex]);
                            resultoflong.push_back(longposting[this->binary_search(longposting,longindex + 4,longindex + simdgalloping::T[0] - 1,shortkey[shortindex])]);
                        }
                    }else{
                        __m128i R = _mm_set1_epi32(*(firstposting + shortindex));
                        __m128i F =  _mm_loadu_si128((__m128i*)(secondposting + longindex + simdgalloping::T[0]));
                        __m128i G =  _mm_loadu_si128((__m128i*)(secondposting + longindex + simdgalloping::T[0] + 4));
                        __m128i J0 = _mm_cmpeq_epi32(F,R);
                        __m128i J1 = _mm_cmpeq_epi32(G,R);
                        __m128i J = _mm_set1_epi32(0);
                        if(_mm_movemask_epi8(_mm_or_si128(J,J0))!=0){
                            resultofshort.push_back(shortposting[shortindex]);
                            resultoflong.push_back(longposting[this->binary_search(longposting,longindex + simdgalloping::T[0],longindex + simdgalloping::T[0] + 4 - 1,shortkey[shortindex])]);
                        }else if(_mm_movemask_epi8(_mm_or_si128(J,J1))!=0){
                            resultofshort.push_back(shortposting[shortindex]);
                            resultoflong.push_back(longposting[this->binary_search(longposting,longindex + simdgalloping::T[0] + 4,longindex + simdgalloping::T[1] - 1,shortkey[shortindex])]);
                        }
                    }
                }else{
                    if(longposting[longindex - 1 + simdgalloping::T[2]] >= shortposting[shortindex]){
                        __m128i R = _mm_set1_epi32(*(firstposting + shortindex));
                        __m128i F =  _mm_loadu_si128((__m128i*)(secondposting + longindex + simdgalloping::T[1]));
                        __m128i G =  _mm_loadu_si128((__m128i*)(secondposting + longindex + simdgalloping::T[1] + 4));
                        __m128i J0 = _mm_cmpeq_epi32(F,R);
                        __m128i J1 = _mm_cmpeq_epi32(G,R);
                        __m128i J = _mm_set1_epi32(0);
                        if(_mm_movemask_epi8(_mm_or_si128(J,J0))!=0){
                            resultofshort.push_back(shortposting[shortindex]);
                            resultoflong.push_back(longposting[this->binary_search(longposting,longindex + simdgalloping::T[1],longindex + simdgalloping::T[1] + 4 - 1,shortkey[shortindex])]);
                        }else if(_mm_movemask_epi8(_mm_or_si128(J,J1))!=0){
                            resultofshort.push_back(shortposting[shortindex]);
                            resultoflong.push_back(longposting[this->binary_search(longposting,longindex + simdgalloping::T[1] + 4,longindex + simdgalloping::T[2] - 1,shortkey[shortindex])]);
                        }
                    }else{
                        __m128i R = _mm_set1_epi32(*(firstposting + shortindex));
                        __m128i F =  _mm_loadu_si128((__m128i*)(secondposting + longindex + simdgalloping::T[2]));
                        __m128i G =  _mm_loadu_si128((__m128i*)(secondposting + longindex + simdgalloping::T[2] + 4));
                        __m128i J0 = _mm_cmpeq_epi32(F,R);
                        __m128i J1 = _mm_cmpeq_epi32(G,R);
                        __m128i J = _mm_set1_epi32(0);
                        if(_mm_movemask_epi8(_mm_or_si128(J,J0)) != 0){
                            resultofshort.push_back(shortposting[shortindex]);
                            resultoflong.push_back(longposting[this->binary_search(longposting,longindex + simdgalloping::T[2], longindex + simdgalloping::T[2] + 4 - 1,shortkey[shortindex])]);
                        }else if(_mm_movemask_epi8(_mm_or_si128(J,J1)) != 0){
                            resultofshort.push_back(shortposting[shortindex]);
                            resultoflong.push_back(longposting[this->binary_search(longposting,longindex + simdgalloping::T[2] + 4,longindex + simdgalloping::T[3] - 1,shortkey[shortindex])]);
                        }
                    }
                }
            };

            SCALAR:
                this->scalar_intersection(shortposting,longposting,shortindex,longindex,resultofshort,resultoflong);
        };

        void SIMD_GALLOPING(std::vector<std::pair<uint32_t,uint64_t>> &shortposting,
                            std::vector<std::pair<uint32_t,uint64_t>> &longposting,
                            std::vector<std::pair<uint32_t,uint64_t>> &resultofshort,
                            std::vector<std::pair<uint32_t,uint64_t>> &resultoflong){

            size_t shortindex = 0;
            size_t longindex = 0;

            if(longposting.size() < simdgalloping::T[3]){
                this->scalar_intersection(shortposting, longposting, shortindex, longindex, resultofshort, resultoflong);
                return;
            }

            std::vector<uint32_t> shortkey;
            std::vector<uint32_t> longkey;

            shortkey.resize(shortposting.size());
            longkey.resize(longposting.size());

            // assign keys for intersection
            for(size_t i = 0 ; i < shortposting.size() ; ++i){
                shortkey[i] = shortposting[i].first;
            }

            for(size_t i = 0 ; i < longposting.size() ; ++i){
                longkey[i] = longposting[i].first;
            }

            uint32_t *firstposting = shortkey.data();
            uint32_t *secondposting = longkey.data();

            for(;shortindex < shortposting.size(); ++shortindex){
//                std::cout<<"shortindex\t"<<shortindex<<std::endl;
                __m128i R = _mm_set1_epi32(*(firstposting + shortindex));
                size_t interval = 0;
                while(longkey[longindex + interval + simdgalloping::T[0]] < shortkey[shortindex]){
                    interval = interval == 0 ? simdgalloping::T[0] : interval << 1;
//                    std::cout<<"double interval to "<<interval<<std::endl;
                    if(longindex + interval > longkey.size()){
                        goto SCALAR;
                    }
                }
//                std::cout<<"longindex\t\t"<<longindex<<std::endl;
                signed long left = interval / 2;
                signed long right = interval;
                signed long mid = left + (right - left) / 2;
                while(left > 0 && right > 0 && left < right){
                    mid = left + (right - left) / 2;
                    if(longposting[longindex + mid - 1 + simdgalloping::T[0]] >= shortposting[shortindex]){
                        right = mid - simdgalloping::T[0];
                    }else if(longposting[longindex + mid - 1 + simdgalloping::T[0]] < shortposting[shortindex]){
                        left = mid + simdgalloping::T[0];
                    }
                }
//                std::cout<<"mid is "<<mid<<std::endl;
                longindex = longindex + mid;
                __m128i F = _mm_loadu_si128((__m128i *) (secondposting + longindex));
                __m128i G = _mm_loadu_si128((__m128i *) (secondposting + longindex + 4));

                __m128i J0 = _mm_cmpeq_epi32(F, R);
                __m128i J1 = _mm_cmpeq_epi32(G, R);
                __m128i judge = _mm_or_si128(J0,J1);
                if(_mm_movemask_epi8(judge)!=0){
                    resultofshort.push_back(shortposting[shortindex]);
                    resultoflong.push_back(longposting[this->binary_search(longposting,longindex,longindex + simdgalloping::T[0],shortposting[shortindex].first)]);
                }
            }
        SCALAR:
            this->scalar_intersection(shortposting,longposting,shortindex,longindex,resultofshort,resultoflong);
        };


    public:
        void intersect(std::vector<std::pair<uint32_t,uint64_t>> & posting1,
                       std::vector<std::pair<uint32_t,uint64_t>> & posting2,
                       std::vector<std::pair<uint32_t,uint64_t>> & result1,
                       std::vector<std::pair<uint32_t,uint64_t>> & result2) {
            if(posting1.size() == 0 || posting2.size() == 0){
                return;
            }

            if(1000 * posting1.size() <= posting2.size() || 1000 * posting2.size() <= posting1.size()){
               if(posting1.size() > posting2.size()){
                   this->SIMD_GALLOPING(posting2,posting1,result2,result1);
                   return;
               } else{
                   this->SIMD_GALLOPING(posting1,posting2,result1,result2);
                   return;
               }
            }

            if(50 * posting1.size() <= posting2.size() || 50 * posting2.size() <= posting1.size()){
                if(posting1.size() > posting2.size()){
                    this->V3(posting2,posting1,result2,result1);
                }else{
                    this->V3(posting1,posting2,result1,result2);
                }
            }

            if(posting1.size() > posting2.size()){
                this->V1(posting2,posting1,result2,result1);
            }else{
                this->V1(posting1,posting2,result1,result2);
            }

        };
        void intersect(std::unordered_map<uint32_t,uint64_t> &posting1,
                       std::unordered_map<uint32_t,uint64_t> &posting2,
                       std::vector<std::pair<uint32_t,uint64_t>> &result1,
                       std::vector<std::pair<uint32_t,uint64_t>> &result2) {
            throw std::runtime_error("simd galloping does not support hash map");
        };
    };
    static const size_t DEMO[] = {8,16,24,32};
    const size_t* textsim::simdgalloping::T = DEMO;
};

#endif //TEXTSIM_SIMDGALLOPING_HPP
