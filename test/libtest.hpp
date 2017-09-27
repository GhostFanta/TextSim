#ifndef TEXTSIM_LIBTEST_HPP
#define TEXTSIM_LIBTEST_HPP

#include "cmph.h"
#include <string.h>

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
  textsim::golomb_rice codec;
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
  textsim::golomb_rice codec;
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
};
}

#endif //TEXTSIM_LIBTEST_HPP
