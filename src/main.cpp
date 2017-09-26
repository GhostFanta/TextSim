#include <iostream>
#include "process.hpp"
#include "libtest.hpp"

int main()
{
   libtest::util::test_bit_vector_read_bits();
   libtest::util::test_bit_vector_write_bit();
   libtest::util::test_bit_vector_write_bits();
   libtest::util::test_bit_vector_read_bit();
   libtest::util::test_write_bits_reverse();
   libtest::util::test_read_bits_reverse();

   libtest::variantbit::test_bitaligned_common();
   libtest::variantbit::test_elias_gamma_dummy();
   libtest::variantbit::test_elias_gamma_short_actual();
   libtest::variantbit::test_elias_gamma();
   libtest::variantbit::test_block();
   libtest::variantbit::test_block_actual_small();
   libtest::variantbit::test_elias_omega_dummy();
   libtest::variantbit::test_elias_omega();
   libtest::variantbit::test_elias_delta_dummy();
   libtest::variantbit::test_elias_delta();
   libtest::variantbit::test_golombrice_dummy();
   libtest::variantbit::test_golombrice();
   return 0;
}
