#pragma once
#include "catch.h"
#include <chrono>
#include <algorithm>
#include <random>

// #define SYMD_USE_TBB 1
#include "../LibSymd/symd.h"

using namespace symd::__internal__;

namespace tests::helpers
{

#ifdef _DEBUG
    constexpr int NUM_ITER = 1;
#else
    constexpr int NUM_ITER = 400;
#endif

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Helper functions
    /////////////////////////////////////////////////////////////////////////////////////////

    /// <summary>
    /// Executes input function for number of times and returns average execution time in ms.
    /// </summary>
    template <typename F>
    static auto measure_execution_time_ms(F&& func, int num_iter = NUM_ITER)
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration;

        for (int i = 0; i < num_iter; i++)
        {
            func();

            auto t2 = std::chrono::high_resolution_clock::now();
            duration = t2 - t1;

            if (duration.count() > 1000.0)
            {
                duration = duration / (i + 1); 
                return duration;
            }
        }

        duration = duration / num_iter;
        return duration;
    }

    /// Applies a unary operation to each element of a vector and returns a new vector with the results.
    /// The type of the output vector can be different from the type of the input vector.
    template <typename U, typename T, typename Operation>
    std::vector<U> apply_unary_op_to_vector(const std::vector<T>& in, Operation&& op)
    {
        std::vector<U> reference(in.size());

        for (size_t i = 0; i < in.size(); i++)
            reference[i] = op(in[i]);

        return reference;
    }


    /// Applies a binary operation to each element of a vector and a given value, and returns a new vector with the results.
    template <typename T, typename Operation>
    std::vector<T> apply_binary_op_to_vector(const std::vector<T>& in, Operation&& op, T x)
    {
        std::vector<T> reference(in.size());

        for (size_t i = 0; i < in.size(); i++)
            reference[i] = op(in[i], x);

        return reference;
    }


    /// Applies a binary operation to a given value and each element of a vector, and returns a new vector with the results.
    template <typename T, typename Operation>
    std::vector<T> apply_binary_op_to_vector(T x, Operation&& op, const std::vector<T>& in)
    {
        std::vector<T> reference(in.size());

        for (size_t i = 0; i < in.size(); i++)
            reference[i] = op(x, in[i]);

        return reference;
    }


    /// Applies a binary operation to each element of two vectors, and returns a new vector with the results.
    template <typename T, typename Operation>
    std::vector<T> apply_binary_op_to_vector(const std::vector<T>& in1, Operation&& op, const std::vector<T>& in2)
    {
        std::vector<T> reference(in1.size());

        for (size_t i = 0; i < in1.size(); i++)
            reference[i] = op(in1[i], in2[i]);

        return reference;
    }


    template <typename T>
    static void require_equal(const SymdRegister<T>& reg, const std::vector<T>& reference)
    {
        std::vector<T> tmpRes(SYMD_LEN + 2);
        tmpRes[0] = (T)0;
        tmpRes[SYMD_LEN + 1] = (T)0;

        reg.store(tmpRes.data() + 1);

        // Does not overwrite nearby locations
        REQUIRE(tmpRes[0] == (T)0);
        REQUIRE(tmpRes[SYMD_LEN + 1] == (T)0);

        for (size_t i = 0; i < SYMD_LEN; i++)
        {
            REQUIRE(reg[i] == reference[i]);

            // Check store as well
            REQUIRE(tmpRes[i + 1] == reference[i]);
        }
    }


    template <typename T>
    static void require_equal(const T* data, const std::vector<T>& ref)
    {
        for (size_t i = 0; i < ref.size(); i++)
            REQUIRE(data[i] == ref[i]);
    }


    template <typename T>
    static void require_equal(const std::vector<T>& data, const std::vector<T>& ref)
    {
        REQUIRE(data.size() == ref.size());
        require_equal(data.data(), ref);
    }


    template <typename T>
    static void require_near(const std::vector<T>& data, const std::vector<T>& ref, T eps)
    {
        REQUIRE(data.size() == ref.size());

        for (size_t i = 0; i < ref.size(); i++)
            REQUIRE(std::abs(data[i] - ref[i]) < eps);
    }


    template <typename T, typename Operation>
    static void check_binary_op_result(const std::vector<T>& in1, Operation&& op, const std::vector<T>& in2)
    {
        REQUIRE(in1.size() == in2.size());

        SymdRegister<T> reg1(in1.data());
        SymdRegister<T> reg2(in2.data());

        require_equal(reg1, in1);
        require_equal(reg2, in2);

        SymdRegister<T> res = op(reg1, reg2);
        std::vector<T> reference(in1.size());

        for (size_t i = 0; i < in1.size(); i++)
            reference[i] = op(in1[i], in2[i]);

        require_equal(res, reference);
    }


    template <typename T, typename Operation>
    static void check_unary_op_result(Operation&& op, const std::vector<T>& in)
    {
        SymdRegister<T> reg(in.data());
        require_equal(reg, in);

        SymdRegister<T> res = op(reg);
        std::vector<T> reference(in.size());

        for (size_t i = 0; i < in.size(); i++)
            reference[i] = op(in[i]);

        require_equal(res, reference);
    }


    template <typename T, typename Operation>
    static void check_cmp_op_result(const std::vector<T>& in1, Operation&& op, const std::vector<T>& in2)
    {
        REQUIRE(in1.size() == in2.size());

        SymdRegister<T> reg1(in1.data());
        SymdRegister<T> reg2(in2.data());

        require_equal(reg1, in1);
        require_equal(reg2, in2);

        SymdRegister<T> reg = op(reg1, reg2);
        std::vector<bool> reference(in1.size());

        for (std::size_t i = 0; i < in1.size(); i++)
            reference[i] = op(in1[i], in2[i]);

        std::vector<T> tmpRes(SYMD_LEN + 2);
        tmpRes[0] = (T)0;
        tmpRes[SYMD_LEN + 1] = (T)0;

        reg.store(tmpRes.data() + 1);

        // Does not overwrite nearby locations
        REQUIRE(tmpRes[0] == (T)0);
        REQUIRE(tmpRes[SYMD_LEN + 1] == (T)0);

        bool valid = true;

        for (std::size_t i = 0; i < SYMD_LEN; i++)
        {
            // FLOATS
            if constexpr (std::is_floating_point_v<T>)
            {
                if (reference[i] && !std::isnan(reg[i]))
                    valid = false;
                else if (!reference[i] && std::isnan(reg[i]))
                    valid = false;

                // Check store as well
                if (reference[i] && !std::isnan(tmpRes[i + 1]))
                    valid = false;
                else if (!reference[i] && std::isnan(tmpRes[i + 1]))
                    valid = false;
            }
            // INTS
            else if constexpr (std::is_integral_v<T>)
            {
                if (reference[i] && reg[i] != -1)
                    valid = false;
                else if (!reference[i] && reg[i] != 0)
                    valid = false;

                // Check store as well
                if (reference[i] && tmpRes[i + 1] != -1)
                    valid = false;
                else if (!reference[i] && tmpRes[i + 1] != 0)
                    valid = false;
            }
            else
            {
                valid = false;
            }
        }

        REQUIRE(valid);
    }


    /// <summary>
    /// Fills input vector with random floats in range 0-255
    /// </summary>
    static void randomize_data(std::vector<float>& data)
    {
        std::default_random_engine generator;
        std::uniform_real_distribution<float> distribution(0.f, 255.f);

        for (auto& x : data)
            x = distribution(generator);
    }


    /// <summary>
    /// Fills input vector with random ints in range 0-255
    /// </summary>
    static void randomize_data(std::vector<int>& data)
    {
        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(0, 255);

        for (auto& x : data)
            x = distribution(generator);
    }
}