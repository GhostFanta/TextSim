#ifndef TEXTSIM_LIBTEST_HPP
#define TEXTSIM_LIBTEST_HPP

#include "cmph.h"
#include <string.h>

#include "simple9.h"
#include "simple8b.h"
#include "simple16.h"

namespace libtest {
namespace cmph {
void test_cmph() {
  // Creating a filled vector
  unsigned int i = 0;
  const char *vector[] = {"aaaaaaaaaa", "bbbbbbbbbb", "cccccccccc", "dddddddddd", "eeeeeeeeee",
                          "ffffffffff", "gggggggggg", "hhhhhhhhhh", "iiiiiiiiii", "jjjjjjjjjj"};
  unsigned int nkeys = 10;
  FILE *mphf_fd = fopen("temp.mph", "w");
  // Source of keys
  cmph_io_adapter_t *source = cmph_io_vector_adapter((char **) vector, nkeys);

  //Create minimal perfect hash function using the brz algorithm.
  cmph_config_t *config = cmph_config_new(source);
  cmph_config_set_algo(config, CMPH_BRZ);
  cmph_config_set_mphf_fd(config, mphf_fd);
  cmph_t *hash = cmph_new(config);
  cmph_config_destroy(config);
  cmph_dump(hash, mphf_fd);
  cmph_destroy(hash);
  fclose(mphf_fd);

  //Find key
  mphf_fd = fopen("temp.mph", "r");
  hash = cmph_load(mphf_fd);
  while (i < nkeys) {
    const char *key = vector[i];
    unsigned int id = cmph_search(hash, key, (cmph_uint32) strlen(key));
    fprintf(stderr, "key:%s -- hash:%u\n", key, id);
    i++;
  }

  //Destroy hash
  cmph_destroy(hash);
  cmph_io_vector_adapter_destroy(source);
  fclose(mphf_fd);
};
};

namespace fastpfor {

void test_simple8b_actual() {
  std::vector<uint32_t> data = {
      1, 225, 1, 235, 1, 345, 1, 644, 1, 221,
      1, 225, 1, 235, 1, 345, 1, 644, 1, 221,
      1, 225, 1, 235, 1, 345, 1, 644, 1, 221
  };
  FastPForLib::Simple8b<false> codec;
  size_t originsize = data.size();
  size_t intermediatesize = originsize;
  std::vector<uint32_t> intermediate;
  intermediate.resize(intermediatesize);
  std::vector<uint32_t> recoverdata;
  recoverdata.resize(originsize);

  codec.encodeArray(data.data(), data.size(), intermediate.data(), intermediatesize);
  codec.decodeArray(intermediate.data(), intermediate.size(), recoverdata.data(), originsize);
  for (size_t i = 0; i < data.size(); i++) {
    std::cout << "data[" << i << "]\t" << data[i] << "\t";
    std::cout << "recoverdata[" << i << "]\t" << recoverdata[i] << std::endl;
    assert(recoverdata[i] == data[i]);
  }
}

void test_simple16_actual() {
  std::vector<uint32_t> data = {
      1, 225, 1, 235, 1, 345, 1, 644, 1, 221,
      1, 225, 1, 235, 1, 345, 1, 644, 1, 221,
      1, 225, 1, 235, 1, 345, 1, 644, 1, 221
  };
  FastPForLib::Simple16<false> codec;
  size_t originsize = data.size();
  size_t intermediatesize = originsize;
  std::vector<uint32_t> intermediate;
  intermediate.resize(intermediatesize);
  uint32_t *intermediatep = &intermediate[0];
  std::vector<uint32_t> recoverdata;
  recoverdata.resize(originsize);

  codec.encodeArray(&data[0], data.size(), intermediatep, intermediatesize);
  codec.decodeArray(intermediatep, intermediate.size(), &recoverdata[0], originsize);
  for (size_t i = 0; i < data.size(); i++) {
    std::cout << "data[" << i << "]\t" << data[i] << "\t";
    std::cout << "recoverdata[" << i << "]\t" << recoverdata[i] << std::endl;
    assert(recoverdata[i] == data[i]);
  }
}

void test_simple9_actual() {
  std::vector<uint32_t> data = {
      1, 225, 1, 235, 1, 345, 1, 644, 1, 221,
      1, 225, 1, 235, 1, 345, 1, 644, 1, 221,
      1, 225, 1, 235, 1, 345, 1, 644, 1, 221
  };
  FastPForLib::Simple9<false> codec;
  size_t originsize = data.size();
  size_t intermediatesize = originsize;
  std::vector<uint32_t> intermediate;
  intermediate.resize(intermediatesize);
  std::vector<uint32_t> recoverdata;
  recoverdata.resize(originsize);

  codec.encodeArray(data.data(), data.size(), intermediate.data(), intermediatesize);
  codec.decodeArray(intermediate.data(), intermediate.size(), recoverdata.data(), originsize);
  for (size_t i = 0; i < data.size(); i++) {
    std::cout << "data[" << i << "]\t" << data[i] << "\t";
    std::cout << "recoverdata[" << i << "]\t" << recoverdata[i] << std::endl;
    assert(recoverdata[i] == data[i]);
  }
};

void test_variant_byte_actual() {

};

void test_variant_gb_actual() {

};

void test_variant_g8iu_actual() {

};

};
}

#endif //TEXTSIM_LIBTEST_HPP
