#ifndef TEXTSIM_TOOL_HPP
#define TEXTSIM_TOOL_HPP

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <limits>
#include <dirent.h>

#include "common.hpp"

#if defined(_MSC_VER)
/* Microsoft C/C++-compatible compiler */
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
/* GCC-compatible compiler, targeting x86/x86-64 */
#include <x86intrin.h>
#elif defined(__GNUC__) && defined(__ARM_NEON__)
/* GCC-compatible compiler, targeting ARM with NEON */
#include <arm_neon.h>
#elif defined(__GNUC__) && defined(__IWMMXT__)
     /* GCC-compatible compiler, targeting ARM with WMMX */
#include <mmintrin.h>
#elif (defined(__GNUC__) || defined(__xlC__)) && (defined(__VEC__) || defined(__ALTIVEC__))
     /* XLC or GCC-compatible compiler, targeting PowerPC with VMX/VSX */
#include <altivec.h>
#elif defined(__GNUC__) && defined(__SPE__)
     /* GCC-compatible compiler, targeting PowerPC with SPE */
#include <spe.h>
#endif

#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif

namespace textsim{
namespace tool {

inline static std::vector<std::string> string_split(std::string s, const char delimeter) {
  size_t start = 0;
  size_t end = s.find_first_of(delimeter);
  std::vector<std::string> output;
  while (end <= std::string::npos) {
    if (end - start > 0) {
      output.emplace_back(s.substr(start, end - start));
    }
    if (end == std::string::npos) {
      break;
    }
    start = end + 1;
    end = s.find_first_of(delimeter, start);
  }
  return output;
};

inline static void split_string_into_gram_freq(std::string source,
                                               std::string &unigram,
                                               unsigned long &frequency) {
  size_t start = 0;
  size_t end = source.find_first_of('\t');
  unigram = source.substr(start, end - start);
  start = end + 1;
  end = source.find_first_of(' ', start);
  frequency = std::stoul(source.substr(start, end - start));
}

// if return 1, then source is bigram,else it is a unigram
inline static bool gram_length(std::string source) {
  size_t start = 0;
  size_t end = source.find_first_of(' ');
  if (end == std::numeric_limits<size_t>::max()) {
    return 0;
  } else {
    return 1;
  }
}

inline static void split_string_into_unigram1_unigram2(std::string source,
                                                       std::string &unigram1,
                                                       std::string &unigram2
) {
  size_t start = 0;
  size_t end = source.find_first_of(' ');
  unigram1 = source.substr(start, end - start);
  start = end + 1;
  end = source.find_first_of(' ', start);
  unigram2 = source.substr(start, end - start);
}

inline static void split_string_into_unigram1_unigram2_freq(std::string source,
                                                            std::string &unigram1,
                                                            std::string &unigram2,
                                                            unsigned long &freq) {
  size_t start = 0;
  size_t end = source.find_first_of(' ');
  unigram1 = source.substr(start, end - start);
  start = end + 1;
  end = source.find_first_of('\t');
  unigram2 = source.substr(start, end - start);
  start = end + 1;
  end = source.length() - 1;
  std::string freqstr;
  freqstr = source.substr(start, end - start);
  freq = std::stoul(freqstr);
}

inline static void split_3gm_unigrams_freq(std::string source,
                                           std::string &unigram1,
                                           std::string &unigram2,
                                           std::string &unigram3,
                                           unsigned long &freq) {
  size_t start = 0;
  size_t endfreq = source.find_first_of('\t');
  size_t endgram1 = source.find_first_of(' ');
  size_t endgram2 = source.find_first_of(' ', endgram1 + 1);
  unigram1 = source.substr(start, endgram1);
  unigram2 = source.substr(endgram1 + 1, endgram2 - endgram1 - 1);
  unigram3 = source.substr(endgram2 + 1, endfreq - endgram2 - 1);
  freq = std::stoul(source.substr(endfreq + 1, source.length() - endfreq));
}

inline static void split_4gm_unigrams_freq(std::string source,
                                           std::string &unigram1,
                                           std::string &unigram2,
                                           std::string &unigram3,
                                           std::string &unigram4,
                                           unsigned long &freq) {
  size_t start = 0;
  size_t endfreq = source.find_first_of('\t');
  size_t endgram1 = source.find_first_of(' ');
  size_t endgram2 = source.find_first_of(' ', endgram1 + 1);
  size_t endgram3 = source.find_first_of(' ', endgram2 + 1);
  unigram1 = source.substr(start, endgram1);
  unigram2 = source.substr(endgram1 + 1, endgram2 - endgram1 - 1);
  unigram3 = source.substr(endgram2 + 1, endgram3 - endgram2 - 1);
  unigram4 = source.substr(endgram3 + 1, endfreq - endgram3 - 1);
  freq = std::stoul(source.substr(endfreq + 1, source.length() - endfreq));
}

inline static std::vector<std::string> string_split_extraction(std::string s, const char delimeter) {
  size_t start = 0;
  size_t end = s.find_first_of(delimeter);
  std::vector<std::string> output;
  while (end <= std::string::npos) {
    auto gram = s.substr(start, end - start);
    if (gram != "") {
      output.emplace_back(gram);
    }
    if (end == std::string::npos) {
      break;
    }
    start = end + 1;
    end = s.find_first_of(delimeter, start);
  }
  return output;
};

// trim from start
static inline std::string &ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                  std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
  return ltrim(rtrim(s));
}

//remove punctuations from a text
static inline std::string remove_punctuation(std::string &input) {
  std::string result;
  std::remove_copy_if(input.begin(), input.end(),
                      std::back_inserter(result), //Store output
                      std::ptr_fun<int, int>(&std::ispunct)
  );
  return result;
}
}
}

namespace fastdelta {

static void scalar_D1_delta_inplace_x32() {

}

static void scalar_D1_delta_recover_x32() {

}

// Refer from Lemire
static void fast_D1_delta_inplace_x32(std::vector<uint32_t> &input) {
  if (input.size() == 0) {
    return;
  }
  uint32_t *data = input.data();
  unsigned long datasize = input.size();
  __m128i slow = _mm_set1_epi32(0);
  size_t index = 0;
  for (; index < datasize / 4; ++index) {
    __m128i fast = _mm_lddqu_si128((const __m128i *) data + index);
    __m128i delta = _mm_sub_epi32(fast, _mm_or_si128(_mm_slli_si128(fast, 4), _mm_srli_si128(slow, 12)));
    _mm_storeu_si128((__m128i *) data + index, delta);
    slow = fast;
  }
  uint32_t lastsstarter = _mm_extract_epi32(slow, 3);
  for (index = 4 * index; index < datasize; ++index) {
    uint32_t demo = data[index];
    data[index] = demo - lastsstarter;
    lastsstarter = demo;
  }
}

// Refer from Lemire
static void fast_D1_recover_inplace_x32(std::vector<uint32_t> &input) {
  if (input.size() == 0) {
    return;
  }
  uint32_t *data = input.data();
  unsigned long datasize = input.size();
  __m128i slow = _mm_set1_epi32(0);
  size_t index = 0;
  for (; index < datasize / 4; ++index) {
    __m128i fast = _mm_lddqu_si128((const __m128i *) data + index);
    fast = _mm_add_epi32(fast, _mm_srli_si128(slow, 12));
    __m128i recover1 = _mm_add_epi32(fast, _mm_slli_si128(fast, 4));
    __m128i recover2 = _mm_add_epi32(recover1, _mm_slli_si128(fast, 8));
    fast = _mm_add_epi32(recover2, _mm_slli_si128(fast, 12));
    _mm_storeu_si128((__m128i *) data + index, fast);
    slow = _mm_set1_epi32(_mm_extract_epi32(fast, 3));
  }
  uint32_t laststarter = _mm_extract_epi32(slow, 3);
  for (index = 4 * index; index < datasize; ++index) {
    uint32_t demo = data[index];
    data[index] = laststarter + demo;
    laststarter = data[index];
  }
}

// Test pass
static void fast_DM_delta_inplace_x32(std::vector<uint32_t> &input) {
  if (input.size() == 0) {
    return;
  }
  uint32_t *data = input.data();
  unsigned long datasize = input.size();
  __m128i slow = _mm_set1_epi32(0);
  size_t index = 0;
  for (; index < datasize / 4; index++) {
    __m128i fast = _mm_lddqu_si128((const __m128i *) data + index);
    __m128i temp = _mm_sub_epi32(fast, slow);
    slow = _mm_set1_epi32(_mm_extract_epi32(fast, 3));
    _mm_storeu_si128((__m128i *) data + index, temp);
  }
  uint32_t frontier = _mm_extract_epi32(slow, 3);
  int remain = datasize - index * 4;
  switch (remain) {
    case 0:
      break;
    case 1:
      data[index * 4] -= frontier;
      break;
    case 2:
      data[index * 4] -= frontier;
      data[index * 4 + 1] -= frontier;
      break;
    case 3:
      data[index * 4] -= frontier;
      data[index * 4 + 1] -= frontier;
      data[index * 4 + 2] -= frontier;
      break;
  }
}

// Test pass
static void fast_DM_recover_inplace_x32(std::vector<uint32_t> &input) {
  if (input.size() == 0) {
    return;
  }
  uint32_t *data = input.data();
  unsigned long datasize = input.size();
  __m128i slow = _mm_set1_epi32(0);
  size_t index = 0;
  for (; index < datasize / 4; index++) {
    __m128i fast = _mm_lddqu_si128((const __m128i *) data + index);
    __m128i temp = _mm_add_epi32(fast, slow);
    slow = _mm_set1_epi32(_mm_extract_epi32(temp, 3));
    _mm_storeu_si128((__m128i *) data + index, temp);
  }
  uint32_t frontier = _mm_extract_epi32(slow, 3);
  int remain = datasize - index * 4;
  switch (remain) {
    case 0:
      break;
    case 1:
      data[index * 4] += frontier;
      break;
    case 2:
      data[index * 4] += frontier;
      data[index * 4 + 1] += frontier;
      break;
    case 3:
      data[index * 4] += frontier;
      data[index * 4 + 1] += frontier;
      data[index * 4 + 2] += frontier;
      break;
  }
}

static void fast_D2_delta_inplace_x32(std::vector<uint32_t> &input) {
  if (input.size() < 2) {
    return;
  }
  uint32_t *data = input.data();
  unsigned long datasize = input.size();
  size_t index = 0;
  __m128i slow = _mm_set1_epi32(0);
  for (; index < datasize / 4; ++index) {
    __m128i temp = _mm_srli_si128(slow, 8);
    __m128i cur = _mm_lddqu_si128((const __m128i *) data + index);
    __m128i curtemp = _mm_or_si128(_mm_slli_si128(cur, 8), temp);
    _mm_storeu_si128((__m128i *) data + index, _mm_sub_epi32(cur, curtemp));
    slow = cur;
  }
  uint32_t xsub[] = {_mm_extract_epi32(slow, 2), _mm_extract_epi32(slow, 3)};
  int remain = datasize - index * 4;
  switch (remain) {
    case 0:
      break;
    case 1 :
      data[index * 4] -= xsub[0];
      break;
    case 2:
      data[index * 4] -= xsub[0];
      data[index * 4 + 1] -= xsub[1];
      break;
    case 3:
      data[index * 4 + 2] -= data[index * 4];
      data[index * 4] -= xsub[0];
      data[index * 4 + 1] -= xsub[1];
      break;

  }
}

static void fast_D2_recover_inplace_x32(std::vector<uint32_t> &input) {
  if (input.size() < 2) {
    return;
  }
  uint32_t *data = input.data();
  std::vector<uint32_t> maskarray = {0, 0, 0xffff, 0xffff};
  __m128i mask = _mm_lddqu_si128((const __m128i *) maskarray.data());
  unsigned long datasize = input.size();
  __m128i slow = _mm_set1_epi32(0);
  size_t index = 0;
  for (; index < datasize / 4; ++index) {
    slow = _mm_or_si128(_mm_and_si128(mask, slow), _mm_srli_si128(slow, 8));
    __m128i curr = _mm_lddqu_si128((const __m128i *) data + index);
    curr = _mm_add_epi32(curr, _mm_slli_si128(curr, 8));
    curr = _mm_add_epi32(slow, curr);
    _mm_storeu_si128((__m128i *) data + index, curr);
    slow = curr;
  }
  uint32_t xsub[] = {_mm_extract_epi32(slow, 2), _mm_extract_epi32(slow, 3)};
  int remain = datasize - index * 4;
  switch (remain) {
    case 0:
      break;
    case 1:
      data[index * 4] += xsub[0];
      break;
    case 2:
      data[index * 4] += xsub[0];
      data[index * 4 + 1] += xsub[1];
      break;
    case 3:
      data[index * 4] += xsub[0];
      data[index * 4 + 1] += xsub[1];
      data[index * 4 + 2] += data[index * 4];
      break;
  }
}

// Test pass
static void fast_D4_delta_inplace_x32(std::vector<uint32_t> &input) {
  if (input.size() == 0) {
    return;
  }
  uint32_t *data = input.data();
  unsigned long datasize = input.size();
  __m128i slow = _mm_set1_epi32(0);
  size_t index = 0;
  for (; index < datasize / 4; ++index) {
    __m128i fast = _mm_lddqu_si128((const __m128i *) data + index);
    __m128i temp = _mm_sub_epi32(fast, slow);
    _mm_storeu_si128((__m128i *) data + index, temp);
    slow = fast;
  }
  uint32_t *remaindersub = (uint32_t *) malloc(sizeof(uint32_t) * 4);
  _mm_storeu_si128((__m128i *) remaindersub, slow);
  int remain = datasize - index * 4;
  switch (remain) {
    case 0:
      break;
    case 1:
      data[index * 4] -= remaindersub[0];
      break;
    case 2:
      data[index * 4] -= remaindersub[0];
      data[index * 4 + 1] -= remaindersub[1];
      break;
    case 3:
      data[index * 4] -= remaindersub[0];
      data[index * 4 + 1] -= remaindersub[1];
      data[index * 4 + 2] -= remaindersub[2];
      break;
  }
}

// Test pass
static void fast_D4_recover_inplace_x32(std::vector<uint32_t> &input) {
  if (input.size() == 0) {
    return;
  }
  uint32_t *data = input.data();
  unsigned long datasize = input.size();
  __m128i slow = _mm_set1_epi32(0);
  size_t index = 0;
  for (; index < datasize / 4; ++index) {
    __m128i fast = _mm_lddqu_si128((const __m128i *) data + index);
    fast = _mm_add_epi32(fast, slow);
    _mm_storeu_si128((__m128i *) data + index, fast);
    slow = fast;
  }
  uint32_t *remaindersub = (uint32_t *) malloc(sizeof(uint32_t) * 4);
  _mm_storeu_si128((__m128i *) remaindersub, slow);
  int remain = datasize - index * 4;
  switch (remain) {
    case 0:
      break;
    case 1:
      data[index * 4] += remaindersub[0];
      break;
    case 2:
      data[index * 4] += remaindersub[0];
      data[index * 4 + 1] += remaindersub[1];
      break;
    case 3:
      data[index * 4] += remaindersub[0];
      data[index * 4 + 1] += remaindersub[1];
      data[index * 4 + 2] += remaindersub[2];
      break;
  }
}
};

#endif //TEXTSIM_TOOL_HPP
