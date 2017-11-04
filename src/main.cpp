#include <iostream>
#include "process.hpp"
#include "libtest.hpp"
#include <chrono>

bool descriptors[256];
void generate_descriptor(uint8_t cur, uint8_t offset){
  if(offset >= 8){
    descriptors[cur] = true;
    return;
  }
  cur &= ~(1 << (offset));
  generate_descriptor(cur,offset + 1);
  generate_descriptor(cur,offset + 2);
  generate_descriptor(cur,offset + 3);
  generate_descriptor(cur,offset + 4);
}

bool descriptorcheck(uint8_t input){
  int prev = 0;
  int cur = 0;
  for(int i = 0 ; i < 8 ; i++){
    if(!((1 << i) & input)){
      prev = cur;
      cur = i;
      std::cout<<prev<<"--"<<cur<<std::endl;
      if(cur - prev > 4){
        assert(false);
      }
    }
  }
  return true;
}

int main()
{
//  generate_descriptor(0xff,0);
//  for(int i = 0 ; i < 256 ; i++){
//    if(descriptors[i]) {
//      std::bitset<8> a(i);
//      std::cout << a << std::endl;
//      descriptorcheck(i);
//    }
//  }

  uint32_t data = 0xaabbccdd;

  __m128i a = _mm_set_epi8(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xcc,0xbb,0xbb,0xbb,0xaa,0xaa);
//  __m128i a = _mm_set_epi8(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xee,0xdd,0xcc,0xbb,0xaa);
  __m128i mask = _mm_set_epi8(-1,-1,-1,-1,-1,-1,-1,5,-1,4,3,2,-1,-1,1,0);
//  __m128i mask = _mm_set_epi8(-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,1,2,3,4,5);
//  __m128i a = _mm_set_epi8(0xaa,0xaa,0xbb,0xbb,    0xbb,0xcc,0x00,0x00,     0x00,0x00,0x00,0x00,    0x00,0x00,0x00,0x00);
//  __m128i mask = _mm_set_epi8(0,1,-1,-1,    2,3,4,-1,    5,-1,-1,-1,    -1,-1,-1,-1);
//  _mm_shuffle_epi8(a,mask);

//   libtest::util::test_bit_vector_read_bits();
//   libtest::util::test_bit_vector_write_bit();
//   libtest::util::test_bit_vector_write_bits();
//   libtest::util::test_bit_vector_read_bit();
//   libtest::util::test_write_bits_reverse();
//   libtest::util::test_read_bits_reverse();
//   libtest::intersection::test_intersection_galloping();
//   libtest::snippet::test_sort_pair();
//   libtest::variantbit::test_bitaligned_common();
//   libtest::variantbit::test_elias_gamma_dummy();
//   libtest::variantbit::test_elias_gamma_short_actual();
//   libtest::variantbit::test_elias_gamma();
//   libtest::variantbit::test_block_2();
//   libtest::variantbit::test_block_4();
//   libtest::variantbit::test_block_3();
//   libtest::variantbit::test_block_5();
//   libtest::variantbit::test_block_6();
//   libtest::variantbit::test_block_actual_small();
//   libtest::variantbit::test_elias_omega_dummy();
//   libtest::variantbit::test_elias_omega();
//   libtest::variantbit::test_elias_delta_dummy();
//   libtest::variantbit::test_elias_delta();
//   libtest::variantbyte::test_variantgb_lessthan4();
//   libtest::variantbyte::test_variant_byte_actual();
//   libtest::variantbyte::test_variant_gb_actual();
//   libtest::fastpfor::test_simple9_wrapped();
//   libtest::fastpfor::test_simple16_wrapped();
//   libtest::intersection::test_intersection_simdgalloping_V1();
//   libtest::intersection::test_intersection_simdgalloping_V1_actual();
//   libtest::intersection::test_intersection_simdgalloping_V1_degrade();
//   libtest::intersection::test_intersection_simdgalloping_V3();
//   libtest::intersection::test_intersection_simdgalloping_V3_degrade();
//   libtest::intersection::test_intersection_simdgalloping_V3_actual();
//   libtest::intersection::test_intersection_simdgalloping_SIMD_degrade();
//   libtest::intersection::test_intersection_simdgalloping_SIMD_actual();
//   libtest::intersection::test_intersection_simdgalloping_SIMD_actual();
//   libtest::simdcompinter::test_simdintersection();
//   libtest::simdcompinter::test_simdintersection();
//   libtest::simdcompinter::test_vgb();
//   libtest::simdcompinter::test_varintG8IU();
//   libtest::codecspeed::test_speed();
//   libtest::codecspeed::test_variantgb();
//   libtest::codecspeed::test_variantgb_actual();
//   libtest::codecspeed::test_variantgb();
//  libtest::variantbyte::test_variant_g8iu_actual_encode();

//    auto t1 = std::chrono::high_resolution_clock::now();
//    for(size_t i = 0 ; i < 100 ;++i){
//        libtest::intersection::test_intersection_linear();
//    }
//    auto t2 = std::chrono::high_resolution_clock::now();
//    std::cout<<"linear:\t"<<std::chrono::duration_cast <std::chrono::microseconds>(t2 - t1).count()<<std::endl;
//
//    auto t5 = std::chrono::high_resolution_clock::now();
//    for(size_t i = 0 ; i < 100 ;++i){
//        libtest::intersection::test_intersection_simdgalloping_V1_actual();
//    }
//    auto t6 = std::chrono::high_resolution_clock::now();
//    std::cout<<"V1:\t"<<std::chrono::duration_cast <std::chrono::microseconds>(t6 - t5).count()<<std::endl;
//
//    auto t7 = std::chrono::high_resolution_clock::now();
//    for(size_t i = 0 ; i < 100 ;++i){
//        libtest::intersection::test_intersection_simdgalloping_V3_actual();
//    }
//    auto t8 = std::chrono::high_resolution_clock::now();
//    std::cout<<"V3:\t"<<std::chrono::duration_cast <std::chrono::microseconds>(t8 - t7).count()<<std::endl;
//
//    auto t3 = std::chrono::high_resolution_clock::now();
//    for(size_t i = 0 ; i < 100 ;++i){
//        libtest::intersection::test_intersection_galloping();
//    }
//    auto t4 = std::chrono::high_resolution_clock::now();
//    std::cout<<"galloping\t:"<<std::chrono::duration_cast <std::chrono::microseconds>(t4 - t3).count()<<std::endl;



   return 0;
}
