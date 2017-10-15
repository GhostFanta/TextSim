#ifndef TEXTSIM_LIBTEST_HPP
#define TEXTSIM_LIBTEST_HPP

#include "cmph.h"
#include <string.h>
#include <chrono>

#include "memutil.h"

#include "simple9.h"
#include "simple8b.h"
#include "simple16.h"

#include "VarIntG8IU.h"

#include "wordcodec/simple9.hpp"
#include "wordcodec/simple8b.hpp"
#include "wordcodec/simple16.hpp"

#include "bytecodec/variantbyte.hpp"
#include "bytecodec/variantgb.hpp"
#include "bytecodec/variantg8iu.hpp"
#include "bytecodec/variantg8cu.hpp"

#include "bitcodec/eliasgamma.hpp"
#include "bitcodec/eliasomega.hpp"
#include "bitcodec/eliasdelta.hpp"
#include "bitcodec/block.hpp"
#include "bitcodec/golombrice.hpp"

#include "util/bit_vector.hpp"

#include "intersection/galloping.hpp"

#include <memory>
#include <iomanip>
#include <time.h>
#include "codecfactory.h"
#include "util.h"
#include "ztimer.h"
#include "bitpacking.h"
#include "synthetic.h"
#include "cpubenchmark.h"

namespace libtest
{
namespace cmph
{
void test_cmph()
{
  // Creating a filled vector
  unsigned int i = 0;
  const char *vector[] = {"aaaaaaaaaa", "bbbbbbbbbb", "cccccccccc", "dddddddddd", "eeeeeeeeee",
                          "ffffffffff", "gggggggggg", "hhhhhhhhhh", "iiiiiiiiii", "jjjjjjjjjj"};
  unsigned int nkeys = 10;
  FILE *mphf_fd = fopen("temp.mph", "w");
  // Source of keys
  cmph_io_adapter_t *source = cmph_io_vector_adapter((char **)vector, nkeys);

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
  while (i < nkeys)
  {
    const char *key = vector[i];
    unsigned int id = cmph_search(hash, key, (cmph_uint32)strlen(key));
    fprintf(stderr, "key:%s -- hash:%u\n", key, id);
    i++;
  }

  //Destroy hash
  cmph_destroy(hash);
  cmph_io_vector_adapter_destroy(source);
  fclose(mphf_fd);
};
};

namespace util
{

void test_bit_vector_write_bit()
{
  std::vector<uint64_t> intermediate;
  {
    textsim::bit_vector_handler util(intermediate);
    util.write_bit<1>();
    util.write_bit<0>();
    util.write_bit<1>();
  }
  assert(0b1010000000000000000000000000000000000000000000000000000000000000 == intermediate[1]);
  {
    textsim::bit_vector_handler util(intermediate);
    util.write_bit<1>();
    util.write_bit<1>();
    util.write_bit<1>();
  }
  assert(0b1011110000000000000000000000000000000000000000000000000000000000 == intermediate[1]);
  std::cout << "---------------write bit pass----------------" << std::endl;
};

void test_bit_vector_write_bits()
{
  std::vector<uint64_t> intermediate;
  {
    textsim::bit_vector_handler util(intermediate);
    util.write_bits(4, 3);
    util.write_bits(4, 4);
  }
  ASSERT(0b1000100000000000000000000000000000000000000000000000000000000000 == intermediate[1], "member not match");
  ASSERT(2 == intermediate.size(), "size not match");
  {
    textsim::bit_vector_handler util(intermediate);
    util.write_bits(9, 60);
  }
  ASSERT(3 == intermediate.size(), "size not match");
  ASSERT(0b0010000000000000000000000000000000000000000000000000000000000000 == intermediate[2], "member not match");
  std::cout << "---------------test write bits pass---------------" << std::endl;
};

void test_bit_vector_read_bit()
{
  std::vector<uint64_t> intermediate;
  {
    textsim::bit_vector_handler util(intermediate);
    util.write_bit<1>();
    util.write_bit<0>();
    util.write_bit<1>();
  }
  {
    textsim::bit_vector_handler util(intermediate);
    assert(util.read_bit() == 1);
    assert(util.read_bit() == 0);
    assert(util.read_bit() == 1);
  }
  {
    textsim::bit_vector_handler util(intermediate);
    util.write_bits(0b0010000000000000000000000000000000000000000000000000000000000000, 64);
    util.write_bits(0b111, 3);
  }
  {
    textsim::bit_vector_handler util(intermediate);
    assert(util.read_bit() == 1);
    assert(util.read_bit() == 1);
    assert(util.read_bit() == 1);
    for (size_t i = 0; i < 61; ++i)
    {
      assert(util.read_bit() == 0);
    }
    assert(util.read_bit() == 1);
    assert(util.read_bit() == 0);
    assert(util.read_bit() == 0);
  }
  std::cout << "---------------read bit pass-------------" << std::endl;
};

void test_bit_vector_read_bits()
{
  std::vector<uint64_t> intermediate;
  {
    textsim::bit_vector_handler util(intermediate);
    util.write_bits(0b00011100, 8);
    ASSERT(util.read_bits(8) == 0b00011100, "read bits within buffer not match");
  }
  {
    textsim::bit_vector_handler util(intermediate);
    util.write_bits(0b101010101010101010101010101010101010101010101010101010101010101, 63);
    util.write_bits(0b000111, 6);
    ASSERT(util.read_bits(6) == 0b000111, "accross border failed");
    util.clear();
  }
  {
    textsim::bit_vector_handler util(intermediate);
    util.write_bits(0b101010101010100000000000000000000000000000000000001010101010101, 63);
    util.write_bits(0b111000, 6);
  }
  {
    textsim::bit_vector_handler util(intermediate);
    ASSERT(util.read_bits(5) == 0b11000, "accross border partial failed");
    ASSERT(util.read_bit() == 1, "read 1 failed");
    ASSERT(util.read_bits(6) == 0b010101, "read partial faild");
    util.clear();
  }
  {
    textsim::bit_vector_handler util(intermediate);
    util.write_bits(0b101010101010100000000000000000000000000000000000001010101010101, 63);
    util.write_bits(0b111000, 6);
    ASSERT(util.read_bits(8) == 0b01111000, "Read cross border directly failed");
  }
  std::cout << "---------------read bits pass-----------" << std::endl;
}

void test_deal_with_actual()
{
  std::vector<uint64_t> intermediate;
  {
    textsim::bit_vector_handler util(intermediate);
    util.write_bits(0b00011100, 8);
    ASSERT(util.read_bits(8) == 0b00011100, "read bits within buffer not match");
  }
  {
    textsim::bit_vector_handler util(intermediate);
    util.write_bits(0b101010101010101010101010101010101010101010101010101010101010101, 63);
    util.write_bits(0b000111, 6);
    ASSERT(util.read_bits(6) == 0b000111, "accross border failed");
    util.clear();
  }
  {
    textsim::bit_vector_handler util(intermediate);
    util.write_bits(0b101010101010100000000000000000000000000000000000001010101010101, 63);
    util.write_bits(0b111000, 6);
  }
  {
    textsim::bit_vector_handler util(intermediate);
    ASSERT(util.read_bits(5) == 0b11000, "accross border partial failed");
    ASSERT(util.read_bit() == 1, "read 1 failed");
    ASSERT(util.read_bits(6) == 0b010101, "read partial faild");
    util.clear();
  }
  {
    textsim::bit_vector_handler util(intermediate);
    util.write_bits(0b101010101010100000000000000000000000000000000000001010101010101, 63);
    util.write_bits(0b111000, 6);
    ASSERT(util.read_bits(8) == 0b01111000, "Read cross border directly failed");
  }
  std::cout << "---------------read bits pass-----------" << std::endl;
};

void test_write_bits_reverse(){
  std::vector<uint64_t> store;
  {
    textsim::bit_vector_handler util(store);
    util.write_bits_reverse(0b111000,6);
  }
  {
    textsim::bit_vector_handler util(store);
    ASSERT(util.read_bits(6) == 0b000111,"reverse writing failed");
  }
  std::cout<<"----------------write bits reverse pass---------------"<<std::endl;

}

void test_read_bits_reverse(){
   std::vector<uint64_t> store;
  {
    textsim::bit_vector_handler util(store);
    util.write_bits_reverse(0b111000,6);
    util.write_bits_reverse(0b11110001111000,14);
  }
  {
    textsim::bit_vector_handler util(store);
    ASSERT(util.read_bits_reverse(14) == 0b11110001111000,"reverse read failed");
    ASSERT(util.read_bits_reverse(6) == 0b111000,"reverse read failed");
  }
  std::cout<<"----------------read bits reverse pass---------------"<<std::endl;
}
};

namespace snippet
{

void test_sort_pair(){

  struct paircomparator{
    bool operator()(const std::pair<uint64_t ,uint64_t > &a, const std::pair<uint64_t,uint64_t> &b){
      return a.first < b.first;
    }
  };

  std::vector<std::pair<uint64_t, uint64_t>> container;
  container.push_back(std::pair<uint64_t,uint64_t>({1,2}));
  container.push_back(std::pair<uint64_t,uint64_t>({3,2}));
  container.push_back(std::pair<uint64_t,uint64_t>({9,2}));
  container.push_back(std::pair<uint64_t,uint64_t>({2,2}));
  container.push_back(std::pair<uint64_t,uint64_t>({31,2}));
  container.push_back(std::pair<uint64_t,uint64_t>({21,2}));
  container.push_back(std::pair<uint64_t,uint64_t>({4,2}));

  std::sort(container.begin(),container.end(),paircomparator());
  for(auto i : container){
    std::cout<<i.first<<"\t"<<i.second<<std::endl;
  }

}

}

namespace variantbit
{

void test_bitaligned_common(){
  textsim::bitalignedcommon test;
  ASSERT(test._log_2_floor(1) == 0,"log2 floor failed");
  ASSERT(test._log_2_floor(4) == 2,"log2 floor failed");
  ASSERT(test._log_2_floor(7) == 2,"log2 floor failed");
  ASSERT(test._log_2_floor(8) == 3,"log2 floor failed");
  ASSERT(test._log_2_floor(15) == 3,"log2 floor failed");
  ASSERT(test._log_2_floor(17) == 4,"log2 floor failed");
  ASSERT(test._log_2_floor(31) == 4,"log2 floor failed");
  ASSERT(test._log_2_floor(32) == 5,"log2 floor failed");
  ASSERT(test._log_2_floor(63) == 5,"log2 floor failed");
  ASSERT(test._log_2_floor(64) == 6,"log2 floor failed");
}

void test_elias_gamma_dummy()
{
  textsim::elias_gamma codec;
  std::vector<uint32_t> data =
      {
          1, 2, 3, 4, 5};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    assert(data[i] == recover[i]);
  }

  std::cout << "---------------gamma dummy pass------------------" << std::endl;
};

void test_elias_gamma_short_actual()
{
  textsim::elias_gamma codec;
  std::vector<uint32_t> data =
      {
          1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
          1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
          1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265
      };
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  ASSERT(data.size() == recover.size(), "recover size not equal to data");
  for (size_t i = 0; i < data.size(); i++)
  {
    assert(data[i] == recover[i]);
  }
  std::cout << "---------------gamma short actual passed------------------" << std::endl;
};

void test_elias_gamma()
{
  textsim::elias_gamma codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
  }
  std::cout<<"---------------test elias gamma pass------------------"<<std::endl;
}


void test_elias_delta_dummy(){
  textsim::elias_delta codec;
  std::vector<uint32_t> data =
      {1,2,3,4,5};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
  }
}

void test_elias_delta()
{
  textsim::elias_delta codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
  }
  std::cout<<"---------------elias delta pass---------------"<<std::endl;
}

void test_elias_omega_dummy(){
  textsim::elias_omega codec;
  std::vector<uint32_t> data =
      {1,2,3,4,5};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
  }
}

void test_elias_omega()
{
  textsim::elias_omega codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
  }
}

void test_block_actual_small()
{
  textsim::block<2> codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    try
    {
      ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
    }
    catch (exception &e)
    {
      textsim::logger::show_uint64t_binary(data[i], "original:\t");
      textsim::logger::show_uint64t_binary(recover[i], "recover:\t");
    }
  }
}

void test_block_2()
{
  textsim::block<2> codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    try
    {
      ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
    }
    catch (exception &e)
    {
      textsim::logger::show_uint64t_binary(data[i], "original:\t");
      textsim::logger::show_uint64t_binary(recover[i], "recover:\t");
    }
  }
  std::cout<<"---------------test block 2 pass-------"<<std::endl;
};

void test_block_3()
{
  textsim::block<3> codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    try
    {
      ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
    }
    catch (exception &e)
    {
      textsim::logger::show_uint64t_binary(data[i], "original:\t");
      textsim::logger::show_uint64t_binary(recover[i], "recover:\t");
    }
  }
  std::cout<<"---------------test block 3 pass-------"<<std::endl;
}

void test_block_4()
{
  textsim::block<4> codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    try
    {
      ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
    }
    catch (exception &e)
    {
      textsim::logger::show_uint64t_binary(data[i], "original:\t");
      textsim::logger::show_uint64t_binary(recover[i], "recover:\t");
    }
  }
  std::cout<<"---------------test block 4 pass-------"<<std::endl;
}
void test_block_5()
{
  textsim::block<5> codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    try
    {
      ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
    }
    catch (exception &e)
    {
      textsim::logger::show_uint64t_binary(data[i], "original:\t");
      textsim::logger::show_uint64t_binary(recover[i], "recover:\t");
    }
  }
  std::cout<<"---------------test block 5 pass-------"<<std::endl;
}
void test_block_6()
{
  textsim::block<6> codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    try
    {
      ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
    }
    catch (exception &e)
    {
      textsim::logger::show_uint64t_binary(data[i], "original:\t");
      textsim::logger::show_uint64t_binary(recover[i], "recover:\t");
    }
  }
  std::cout<<"---------------test block 6 pass-------"<<std::endl;
}

void test_golombrice_dummy(){
  textsim::golomb_rice<4> codec;
  std::vector<uint32_t> data =
      {
          1,2,3,4,5
      };
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
  }
}

void test_golombrice()
{
  textsim::golomb_rice<4> codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
  }
};
}

namespace variantbyte
{

// Passes
void test_variant_byte_actual()
{
  textsim::vbyte codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  std::vector<uint8_t> intermediate;
  size_t originalsize = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x8(data, originalsize, intermediate, intermediatesize);
  std::vector<uint32_t> recover;
  size_t expectednum = data.size();
  codec.decode_x8(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    assert(recover[i] == data[i]);
  }
};

// Passes
void test_variant_byte_interface()
{
  textsim::intermediate_x8 *codec = new textsim::vbyte();
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  std::vector<uint8_t> intermediate;
  size_t originalsize = data.size();
  size_t intermediatesize = intermediate.size();
  codec->encode_x8(data, originalsize, intermediate, intermediatesize);
  std::vector<uint32_t> recover;
  size_t expectednum = data.size();
  codec->decode_x8(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    assert(recover[i] == data[i]);
  }
}

// Passes
void test_variant_gb_actual()
{
  textsim::variantgb codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  std::vector<uint8_t> intermediate;
  std::vector<uint32_t> recover;
  size_t expectednum = data.size();
  size_t originalsize = expectednum;
  size_t intermediatesize = originalsize;
  codec.encode_x8(data, originalsize, intermediate, intermediatesize);
  codec.decode_x8(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    assert(data[i] == recover[i]);
  }
};

void test_variantgb_lessthan4(){
   textsim::variantgb codec;
  std::vector<uint32_t> data =
      {
          1,2
      };
  std::vector<uint8_t> intermediate;
  std::vector<uint32_t> recover;
  size_t expectednum = data.size();
  size_t originalsize = expectednum;
  size_t intermediatesize = originalsize;
  codec.encode_x8(data, originalsize, intermediate, intermediatesize);
  codec.decode_x8(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    assert(data[i] == recover[i]);
  }
}

void test_variant_g8iu_actual()
{
  FastPForLib::VarIntG8IU codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  size_t originalnum = data.size();
  size_t recoversize = originalnum;

  std::vector<uint32_t> intermediate;
  intermediate.resize(originalnum * 2 + 1024);
  size_t intermediatesize = intermediate.size();
  std::vector<uint32_t> recover;
  recover.resize(originalnum + 1024);

  codec.encodeArray(data.data(), data.size(), intermediate.data(), intermediatesize);
  codec.decodeArray(intermediate.data(), intermediate.size(), recover.data(), recoversize);

  for (size_t i = 0; i < recover.size(); i++)
  {
    assert(data[i] == recover[i]);
  };
};

void test_variant_g8cu_actual(){

};
};

namespace intersection
{
void test_intersection_galloping(){
  textsim::gallop interstc;
  std::vector<std::pair<uint32_t, uint64_t>> posting1 = {{1,1},{2,3},{4,5},{7,8},{9,10},{11,12},{13,5},{14,12}};
  std::vector<std::pair<uint32_t, uint64_t>> posting2 = {{2,3},{4,5},{7,8},{9,10},{13,12}};
  std::vector<std::pair<uint32_t, uint64_t>> posting3;
  std::vector<std::pair<uint32_t, uint64_t>> posting4;

  interstc.intersect(posting1,posting2,posting3,posting4);
  for(auto i : posting3){
    std::cout<<i.first<<"--"<<i.second<<"\t";
  };

  std::cout<<std::endl;
  for(auto i : posting4){
    std::cout<<i.first<<"--"<<i.second<<"\t";
  }
  std::cout<<std::endl;
};

void test_intersection_simdgalloping(){

};
}

namespace framebased
{

using namespace FastPForLib;
using namespace std;

void test_pfor()
{
  std::vector<uint32_t> data = {
      1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
      1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
      1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  PFor codec;
  size_t originsize = data.size();
  size_t intermediatesize = originsize;
  std::vector<uint32_t> intermediate;
  intermediate.resize(intermediatesize);
  uint32_t *intermediatep = &intermediate[0];
  std::vector<uint32_t> recoverdata;
  recoverdata.resize(originsize + 1024);

  codec.encodeArray(data.data(), data.size(), intermediate.data(), intermediatesize);
  codec.decodeArray(intermediate.data(), intermediate.size(), recoverdata.data(), originsize);
  for (size_t i = 0; i < data.size(); i++)
  {
    assert(recoverdata[i] == data[i]);
  }
  recoverdata.shrink_to_fit();
}
}

namespace fastpfor
{

// Passes
void test_simple8b_actual()
{
  std::vector<uint32_t> data = {
      1, 225, 1, 235, 1, 345, 1, 644, 1, 221,
      1, 225, 1, 235, 1, 345, 1, 644, 1, 221,
      1, 225, 1, 235, 1, 345, 1, 644, 1, 221};
  FastPForLib::Simple8b<false> codec;
  size_t originsize = data.size();
  size_t intermediatesize = originsize;
  std::vector<uint32_t> intermediate;
  intermediate.resize(intermediatesize);
  std::vector<uint32_t> recoverdata;
  recoverdata.resize(originsize);

  codec.encodeArray(data.data(), data.size(), intermediate.data(), intermediatesize);
  codec.decodeArray(intermediate.data(), intermediate.size(), recoverdata.data(), originsize);
  for (size_t i = 0; i < data.size(); i++)
  {
    std::cout << "data[" << i << "]\t" << data[i] << "\t";
    std::cout << "recoverdata[" << i << "]\t" << recoverdata[i] << std::endl;
    assert(recoverdata[i] == data[i]);
  }
};

// Passes
void test_simple8b_wapped()
{
  std::vector<uint32_t> data = {
      1, 225, 1, 235, 1, 345, 1, 644, 1, 221,
      1, 225, 1, 235, 1, 345, 1, 644, 1, 221,
      1, 225, 1, 235, 1, 345, 1, 644, 1, 221};
  textsim::simple8bcodec codec;
  size_t originsize = data.size();
  size_t intermediatesize = originsize;
  std::vector<uint32_t> intermediate;
  intermediate.resize(intermediatesize);
  std::vector<uint32_t> recoverdata;
  recoverdata.resize(originsize);

  codec.encode_x32(data, originsize, intermediate, intermediatesize);
  codec.decode_x32(intermediate, intermediatesize, recoverdata, originsize);
  for (size_t i = 0; i < data.size(); i++)
  {
    std::cout << "data[" << i << "]\t" << data[i] << "\t";
    std::cout << "recoverdata[" << i << "]\t" << recoverdata[i] << std::endl;
    assert(recoverdata[i] == data[i]);
  }
}

// Passes
void test_simple16_dummy()
{
  std::vector<uint32_t> data = {
      1, 225, 1, 235, 1, 345, 1, 644, 1, 221};
  FastPForLib::Simple16<false> codec;
  size_t originsize = data.size();
  std::vector<uint32_t> intermediate(data.size());
  size_t intermediatesize = intermediate.size();
  uint32_t *intermediatep = &intermediate[0];
  std::vector<uint32_t> recoverdata;
  recoverdata.resize(originsize + 1024);

  codec.encodeArray(&data[0], data.size(), intermediate.data(), intermediatesize);
  codec.decodeArray(intermediate.data(), intermediate.size(), recoverdata.data(), originsize);
  for (size_t i = 0; i < data.size(); i++)
  {
    assert(data[i] == recoverdata[i]);
  }
  recoverdata.shrink_to_fit();
};

// Passes
void test_simple16_actual()
{
  std::vector<uint32_t> data = {
      1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
      1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
      1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  FastPForLib::Simple16<false> codec;
  size_t originsize = data.size();
  size_t intermediatesize = originsize;
  std::vector<uint32_t> intermediate;
  intermediate.resize(intermediatesize);
  uint32_t *intermediatep = &intermediate[0];
  std::vector<uint32_t> recoverdata;
  recoverdata.resize(originsize + 1024);

  codec.encodeArray(data.data(), data.size(), intermediate.data(), intermediatesize);
  codec.decodeArray(intermediate.data(), intermediate.size(), recoverdata.data(), originsize);
  for (size_t i = 0; i < data.size(); i++)
  {
    std::cout << "data[" << i << "]\t" << data[i] << "\t";
    std::cout << "recoverdata[" << i << "]\t" << recoverdata[i] << std::endl;
    assert(recoverdata[i] == data[i]);
  }
  recoverdata.shrink_to_fit();
}

void test_simple16_wrapped(){
  std::vector<uint32_t> data = {
      1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
      1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
      1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  textsim::simple16codec codec;
  size_t originsize = data.size();
  size_t intermediatesize = originsize;
  std::vector<uint32_t> intermediate;
  intermediate.resize(intermediatesize);
  std::vector<uint32_t> recoverdata;
  recoverdata.resize(originsize + 1024);

  codec.encode_x32(data, originsize, intermediate, intermediatesize);
  codec.decode_x32(intermediate, intermediatesize, recoverdata, originsize);
  for (size_t i = 0; i < data.size(); i++)
  {
    std::cout << "data[" << i << "]\t" << data[i] << "\t";
    std::cout << "recoverdata[" << i << "]\t" << recoverdata[i] << std::endl;
    assert(recoverdata[i] == data[i]);
  }
}

// Passes
void test_simple9_dummy()
{
  std::vector<uint32_t> data = {
      1, 1, 1, 1, 1, 1, 1, 1, 1};
  FastPForLib::Simple9<false> codec;
  size_t originsize = data.size();
  size_t intermediatesize = originsize;
  std::vector<uint32_t> intermediate;
  intermediate.resize(intermediatesize);
  std::vector<uint32_t> recoverdata;
  recoverdata.resize(originsize + 1024);

  codec.encodeArray(data.data(), data.size(), intermediate.data(), intermediatesize);
  codec.decodeArray(intermediate.data(), intermediate.size(), recoverdata.data(), originsize);
  for (size_t i = 0; i < data.size(); i++)
  {
    std::cout << "data[" << i << "]\t" << data[i] << "\t";
    std::cout << "recoverdata[" << i << "]\t" << recoverdata[i] << std::endl;
    assert(recoverdata[i] == data[i]);
  }
}

// Passes
void test_simple9_actual()
{
  std::vector<uint32_t> data = {
      1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
      1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
      1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  FastPForLib::Simple9<false> codec;
  size_t originsize = data.size();
  size_t intermediatesize = originsize;
  std::vector<uint32_t> intermediate;
  intermediate.resize(intermediatesize);
  std::vector<uint32_t> recoverdata;
  recoverdata.resize(originsize + 1024);

  codec.encodeArray(data.data(), data.size(), intermediate.data(), intermediatesize);
  codec.decodeArray(intermediate.data(), intermediate.size(), recoverdata.data(), originsize);
  for (size_t i = 0; i < data.size(); i++)
  {
    std::cout << "data[" << i << "]\t" << data[i] << "\t";
    std::cout << "recoverdata[" << i << "]\t" << recoverdata[i] << std::endl;
    assert(recoverdata[i] == data[i]);
  }
};


void test_simple9_wrapped(){
 std::vector<uint32_t> data = {
      1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
      1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
      1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  textsim::simple9codec codec;
  size_t originsize = data.size();
  size_t intermediatesize = originsize;
  std::vector<uint32_t> intermediate;
  intermediate.resize(intermediatesize);
  std::vector<uint32_t> recoverdata;
  recoverdata.resize(originsize + 1024);

  codec.encode_x32(data, originsize, intermediate, intermediatesize);
  codec.decode_x32(intermediate, intermediatesize, recoverdata, originsize);
  for (size_t i = 0; i < data.size(); i++)
  {
    std::cout << "data[" << i << "]\t" << data[i] << "\t";
    std::cout << "recoverdata[" << i << "]\t" << recoverdata[i] << std::endl;
    assert(recoverdata[i] == data[i]);
  }
}
};

namespace codecspeed
{
void test_variantbyte(){
  textsim::elias_delta codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
  }
}

void test_variantgb(){
  textsim::elias_delta codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
  }
}

void test_simple8b(){
  textsim::elias_delta codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
  }
}

void test_simple9(){
  textsim::elias_delta codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
  }
}

void test_simple_16(){
  textsim::elias_delta codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
  }
}

void test_elias_gamma(){
  textsim::elias_delta codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
  }
}

void test_elias_delta(){
  textsim::elias_delta codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
  }
}

void test_elias_omega(){
  textsim::elias_delta codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
  }
}

void test_block(){
  textsim::elias_delta codec;
  std::vector<uint32_t> data =
      {1, 211, 1, 212, 1, 213, 1, 214, 1, 215, 1, 216, 1, 221, 1, 222, 1, 223, 1, 224, 1, 225, 1, 226, 1, 231, 1, 232,
       1, 233, 1, 234, 1, 235, 1, 236, 1, 241, 1, 242, 1, 243, 1, 244, 1, 245, 1, 246, 1, 251, 1, 252, 1, 253, 1, 254,
       1, 255, 1, 256, 1, 261, 1, 262, 1, 263, 1, 264, 1, 265, 1, 266};
  std::vector<uint64_t> intermediate;
  std::vector<uint32_t> recover;
  size_t originalsize = data.size();
  size_t expectednum = data.size();
  size_t intermediatesize = intermediate.size();
  codec.encode_x64(data, originalsize, intermediate, intermediatesize);
  codec.decode_x64(intermediate, intermediatesize, recover, expectednum);
  for (size_t i = 0; i < data.size(); i++)
  {
    ASSERT(data[i] == recover[i], "member " + std::to_string(i) + " not equal");
  }
}

void test_speed(){
  std::cout<<"1000000 elias gamma"<<std::endl;
  auto t1 = std::chrono::high_resolution_clock::now();
  for(size_t i = 0 ; i < 1000000 ; i++){
    test_elias_gamma();
  }
  auto t2 = std::chrono::high_resolution_clock::now();
  std::cout<<std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()<<std::endl;

  std::cout<<"1000000 elias omega"<<std::endl;
  t1 = std::chrono::high_resolution_clock::now();
  for(size_t i = 0 ; i < 1000000 ; i++){
    test_elias_omega();
  }
  t2 = std::chrono::high_resolution_clock::now();
  std::cout<<std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()<<std::endl;

  std::cout<<"1000000 elias delta"<<std::endl;
  t1 = std::chrono::high_resolution_clock::now();
  for(size_t i = 0 ; i < 1000000 ; i++){
    test_elias_delta();
  }
  t2 = std::chrono::high_resolution_clock::now();
  std::cout<<std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()<<std::endl;

  std::cout<<"1000000 block"<<std::endl;
  t1 = std::chrono::high_resolution_clock::now();
  for(size_t i = 0 ; i < 1000000 ; i++){
    test_block();
  }
  t2 = std::chrono::high_resolution_clock::now();
  std::cout<<std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()<<std::endl;


  std::cout<<"1000000 vb"<<std::endl;
  t1 = std::chrono::high_resolution_clock::now();
  for(size_t i = 0 ; i < 1000000 ; i++){
    test_variantbyte();
  }
  t2 = std::chrono::high_resolution_clock::now();
  std::cout<<std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()<<std::endl;

  std::cout<<"1000000 vgb"<<std::endl;
  t1 = std::chrono::high_resolution_clock::now();
  for(size_t i = 0 ; i < 1000000 ; i++){
    test_variantgb();
  }
  t2 = std::chrono::high_resolution_clock::now();
  std::cout<<std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()<<std::endl;

  std::cout<<"1000000 simple9"<<std::endl;
  t1 = std::chrono::high_resolution_clock::now();
  for(size_t i = 0 ; i < 1000000 ; i++){
    test_simple9();
  }
  t2 = std::chrono::high_resolution_clock::now();
  std::cout<<std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()<<std::endl;

  std::cout<<"1000000 simple8b"<<std::endl;
  t1 = std::chrono::high_resolution_clock::now();
  for(size_t i = 0 ; i < 1000000 ; i++){
    test_simple8b();
  }
  t2 = std::chrono::high_resolution_clock::now();
  std::cout<<std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()<<std::endl;

  std::cout<<"1000000 simple16"<<std::endl;
  t1 = std::chrono::high_resolution_clock::now();
  for(size_t i = 0 ; i < 1000000 ; i++){
    test_simple_16();
  }
  t2 = std::chrono::high_resolution_clock::now();
  std::cout<<std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()<<std::endl;
}

}
}

#endif //TEXTSIM_LIBTEST_HPP
