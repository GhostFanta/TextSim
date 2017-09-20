#ifndef TEXTSIM_COMMON_HPP
#define TEXTSIM_COMMON_HPP

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <vector>
#include <bitset>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <chrono>

namespace textsim {
namespace common {
const static size_t UNISIZE = 4210037;
const static size_t BISIZE = 41296941;
const static size_t CONSIZE = 150406843;
const static size_t IDWIDTH = 23;

const static size_t FREQSIZE = 482265;
const static size_t FREQMAXWIDTH = 37;

const static size_t FRONTIERBIT = 19;
const static size_t FREQSIZEUNIBI = 394061;
const static size_t ESCAPEARRAYSIZE = 32;
}

}

#endif //TEXTSIM_COMMON_HPP
