#pragma once
#include "../test_helpers.h"


namespace tests
{
    TEST_CASE("SymdRegister bfloat16 load and store 1")
    {
        std::vector<symd::bfloat16> in_data = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
        symd::__internal__::SymdRegister<symd::bfloat16> reg(in_data.data());

        REQUIRE(reg[0] == 1.0f);
        REQUIRE(reg[1] == 2.0f);
        REQUIRE(reg[2] == 3.0f);
        REQUIRE(reg[3] == 4.0f);

        REQUIRE(reg[4] == 5.0f);
        REQUIRE(reg[5] == 6.0f);
        REQUIRE(reg[6] == 7.0f);
        REQUIRE(reg[7] == 8.0f);

        std::vector<symd::bfloat16> out_data = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
        reg.store(out_data.data());

        REQUIRE(out_data[0].get_float() == 1.0f);
        REQUIRE(out_data[1].get_float() == 2.0f);
        REQUIRE(out_data[2].get_float() == 3.0f);
        REQUIRE(out_data[3].get_float() == 4.0f);

        REQUIRE(out_data[4].get_float() == 5.0f);
        REQUIRE(out_data[5].get_float() == 6.0f);
        REQUIRE(out_data[6].get_float() == 7.0f);
        REQUIRE(out_data[7].get_float() == 8.0f);
    }

    TEST_CASE("Shuffle 16 bit words to convert float to bfloat16")
    {
        // Symd conversion test -> run only on intel
        // std::vector<short> in_data = { 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8 };

        // __m256i a = _mm256_loadu_si256((__m256i*)in_data.data());
        
        // __m256i shuffled_hi = _mm256_shufflehi_epi16(a, 0b00001101);
        // __m256i shuffled_16 = _mm256_shufflelo_epi16(shuffled_hi, 0b00001101);

        // __m256i shuffled_32 = _mm256_shuffle_epi32(shuffled_16, 0b11111000);
        // __m256i shuffled_64 = _mm256_permute4x64_epi64(shuffled_32, 0b11111000);

        // std::vector<short> out_data = { 0, 0, 0, 0, 0, 0, 0, 0 };
        // _mm_store_si128 ((__m128i*)out_data.data(), _mm256_extracti128_si256(shuffled_64, 0));

        // REQUIRE(out_data[0] == 1);
        // REQUIRE(out_data[1] == 2);
        // REQUIRE(out_data[2] == 3);
        // REQUIRE(out_data[3] == 4);

        // REQUIRE(out_data[4] == 5);
        // REQUIRE(out_data[5] == 6);
        // REQUIRE(out_data[6] == 7);
        // REQUIRE(out_data[7] == 8);
    }
}

