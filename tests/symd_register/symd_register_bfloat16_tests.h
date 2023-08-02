#pragma once
#include "../test_helpers.h"


namespace tests
{
    TEST_CASE("SymdRegister bfloat16 load and store 1")
    {
        std::vector<symd::bfloat16> in_data = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
        symd::__internal__::SymdRegister<symd::bfloat16> reg(in_data.data());

        for (int i = 0; i < symd::__internal__::SYMD_LEN; i++)
            REQUIRE((float)reg[i] == (float)in_data[i]);

        std::vector<symd::bfloat16> out_data = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
        reg.store(out_data.data());

        for (int i = 0; i < symd::__internal__::SYMD_LEN; i++)
            REQUIRE((float)out_data[i] == (float)in_data[i]);
    }

    TEST_CASE("SymdRegister bfloat16 load single float to reg")
    {
        symd::__internal__::SymdRegister<symd::bfloat16> reg(symd::bfloat16(7.0f));

        for (int i = 0; i < symd::__internal__::SYMD_LEN; i++)
            REQUIRE(reg[i] == 7.0f);
    }

    TEST_CASE("SymdRegister bfloat16 sub")
    {
        symd::__internal__::SymdRegister<symd::bfloat16> a(symd::bfloat16(10.0f));
        symd::__internal__::SymdRegister<symd::bfloat16> b(symd::bfloat16(3.0f));

        auto res = a - b;

        for (int i = 0; i < symd::__internal__::SYMD_LEN; i++)
            REQUIRE(res[i] == 7.0f);
    }

    TEST_CASE("SymdRegister bfloat16 mul")
    {
        symd::__internal__::SymdRegister<symd::bfloat16> a(symd::bfloat16(10.0f));
        symd::__internal__::SymdRegister<symd::bfloat16> b(symd::bfloat16(3.0f));

        auto res = a * b;

        for (int i = 0; i < symd::__internal__::SYMD_LEN; i++)
            REQUIRE(res[i] == 30.0f);
    }

    TEST_CASE("SymdRegister bfloat16 div")
    {
        symd::__internal__::SymdRegister<symd::bfloat16> a(symd::bfloat16(10.0f));
        symd::__internal__::SymdRegister<symd::bfloat16> b(symd::bfloat16(2.0f));

        auto res = a / b;

        for (int i = 0; i < symd::__internal__::SYMD_LEN; i++)
            REQUIRE(res[i] == 5.0f);
    }

    TEST_CASE("SymdRegister convert bfloat16 to float")
    {
        symd::__internal__::SymdRegister<symd::bfloat16> a(5.0f);
        auto res = a.convert_to<float>();

        for (int i = 0; i < symd::__internal__::SYMD_LEN; i++)
            REQUIRE(res[i] == 5.0f);
    }

    TEST_CASE("SymdRegister convert bfloat16 to double")
    {
        symd::__internal__::SymdRegister<symd::bfloat16> a(5.0f);
        auto res = a.convert_to<double>();

        for (int i = 0; i < symd::__internal__::SYMD_LEN; i++)
            REQUIRE(res[i] == 5.0);
    }

    TEST_CASE("SymdRegister convert bfloat16 to int")
    {
        symd::__internal__::SymdRegister<symd::bfloat16> a(5.3f);
        auto res = a.convert_to<int>();

        for (int i = 0; i < symd::__internal__::SYMD_LEN; i++)
            REQUIRE(res[i] == 5);
    }

    TEST_CASE("SymdRegister convert bfloat16 to unsigned char")
    {
        symd::__internal__::SymdRegister<symd::bfloat16> a(5.3f);
        auto res = a.convert_to<unsigned char>();

        for (int i = 0; i < symd::__internal__::SYMD_LEN; i++)
            REQUIRE(res[i] == 5);
    }

    TEST_CASE("SymdRegister convert float to bfloat16")
    {
        symd::__internal__::SymdRegister<float> a(5.0f);
        auto res = a.convert_to<symd::bfloat16>();

        for (int i = 0; i < symd::__internal__::SYMD_LEN; i++)
            REQUIRE(res[i] == 5.0f);
    }

    TEST_CASE("SymdRegister convert double to bfloat16")
    {
        symd::__internal__::SymdRegister<double> a(5.0);
        auto res = a.convert_to<symd::bfloat16>();

        for (int i = 0; i < symd::__internal__::SYMD_LEN; i++)
            REQUIRE(res[i] == 5.0f);
    }

    TEST_CASE("SymdRegister convert int to bfloat16")
    {
        symd::__internal__::SymdRegister<int> a(5);
        auto res = a.convert_to<symd::bfloat16>();

        for (int i = 0; i < symd::__internal__::SYMD_LEN; i++)
            REQUIRE(res[i] == 5.0f);
    }

    TEST_CASE("SymdRegister convert unsigned char to bfloat16")
    {
        symd::__internal__::SymdRegister<unsigned char> a(5);
        auto res = a.convert_to<symd::bfloat16>();

        for (int i = 0; i < symd::__internal__::SYMD_LEN; i++)
            REQUIRE(res[i] == 5.0f);
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

