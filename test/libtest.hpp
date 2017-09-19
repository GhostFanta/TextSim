#ifndef TEXTSIM_LIBTEST_HPP
#define TEXTSIM_LIBTEST_HPP

#include "cmph.h"
#include <string.h>

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

}

void test_simple16_actual() {

}

void test_simple9_actual() {

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
