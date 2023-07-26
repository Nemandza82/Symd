#pragma once
#include "catch.h"
#include <chrono>
#include <algorithm>
#include <random>

// #define SYMD_USE_TBB 1
#include "../LibSymd/symd.h"

using namespace symd::__internal__;

namespace tests
{

#ifdef _DEBUG
    constexpr int NUM_ITER = 1;
#else
    constexpr int NUM_ITER = 500;
#endif

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Helper functions
    /////////////////////////////////////////////////////////////////////////////////////////

    /// <summary>
    /// Executes input function for number of times and returns average execution time in ms.
    /// </summary>
    template <typename F>
    static auto executionTimeMs(F&& func, int num_iter = NUM_ITER)
    {
        auto t1 = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < num_iter; i++)
        {
            func();
        }

        auto t2 = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> duration = t2 - t1;
        duration = duration / num_iter;

        return duration;
    }

    template <typename T>
    static bool isRegEqualToData(const SymdRegister<T>& reg, const std::vector<T>& reference)
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
            if (reg[i] != reference[i])
                return false;

            // Check store as well
            if (tmpRes[i + 1] != reference[i])
                return false;
        }

        return true;
    }

    template <typename T>
    static bool isRegCmpValid(const SymdRegister<T>& reg, const std::vector<bool>& reference)
    {
        std::vector<T> tmpRes(SYMD_LEN + 2);
        tmpRes[0] = (T)0;
        tmpRes[SYMD_LEN + 1] = (T)0;

        reg.store(tmpRes.data() + 1);

        // Does not overwrite nearby locations
        REQUIRE(tmpRes[0] == (T)0);
        REQUIRE(tmpRes[SYMD_LEN + 1] == (T)0);

        for (std::size_t i = 0; i < SYMD_LEN; i++)
        {
            // FLOATS
            if constexpr (std::is_floating_point_v<T>)
            {
                if (reference[i] && !std::isnan(reg[i]))
                    return false;
                else if (!reference[i] && std::isnan(reg[i]))
                    return false;

                // Check store as well
                if (reference[i] && !std::isnan(tmpRes[i + 1]))
                    return false;
                else if (!reference[i] && std::isnan(tmpRes[i + 1]))
                    return false;
            }
            // INTS
            else if constexpr (std::is_integral_v<T>)
            {
                if (reference[i] && reg[i] != -1)
                    return false;
                else if (!reference[i] && reg[i] != 0)
                    return false;

                // Check store as well
                if (reference[i] && tmpRes[i + 1] != -1)
                    return false;
                else if (!reference[i] && tmpRes[i + 1] != 0)
                    return false;
            }
            else
            {
                return false;
            }
        }

        return true;
    }


    template <typename T, typename Operation>
    std::vector<T> applyOpToVector(const std::vector<T>& in, Operation&& op, T x)
    {
        std::vector<T> reference(in.size());

        for (size_t i = 0; i < in.size(); i++)
            reference[i] = op(in[i], x);

        return reference;
    }


    template <typename T, typename Operation>
    std::vector<T> applyOpToVector(T x, Operation&& op, const std::vector<T>& in)
    {
        std::vector<T> reference(in.size());

        for (size_t i = 0; i < in.size(); i++)
            reference[i] = op(x, in[i]);

        return reference;
    }


    template <typename T, typename Operation>
    std::vector<T> applyOpToVector(const std::vector<T>& in1, Operation&& op, const std::vector<T>& in2)
    {
        std::vector<T> reference(in1.size());

        for (size_t i = 0; i < in1.size(); i++)
            reference[i] = op(in1[i], in2[i]);

        return reference;
    }


    template <typename T, typename Operation>
    static void checkOperationResult(const std::vector<T>& in1, Operation&& op, const std::vector<T>& in2)
    {
        REQUIRE(in1.size() == in2.size());

        SymdRegister<T> reg1(in1.data());
        SymdRegister<T> reg2(in2.data());

        REQUIRE(isRegEqualToData(reg1, in1));
        REQUIRE(isRegEqualToData(reg2, in2));

        SymdRegister<T> res = op(reg1, reg2);
        std::vector<T> reference(in1.size());

        for (size_t i = 0; i < in1.size(); i++)
            reference[i] = op(in1[i], in2[i]);

        REQUIRE(isRegEqualToData(res, reference));
    }


    template <typename T, typename Operation>
    static void checkUnaryOperationResult(Operation&& op, const std::vector<T>& in)
    {
        SymdRegister<T> reg(in.data());
        REQUIRE(isRegEqualToData(reg, in));

        SymdRegister<T> res = op(reg);
        std::vector<T> reference(in.size());

        for (size_t i = 0; i < in.size(); i++)
            reference[i] = op(in[i]);

        REQUIRE(isRegEqualToData(res, reference));
    }


    template <typename T, typename Operation>
    static void checkCmpOperationResult(const std::vector<T>& in1, Operation&& op, const std::vector<T>& in2)
    {
        REQUIRE(in1.size() == in2.size());

        SymdRegister<T> reg1(in1.data());
        SymdRegister<T> reg2(in2.data());

        REQUIRE(isRegEqualToData(reg1, in1));
        REQUIRE(isRegEqualToData(reg2, in2));

        SymdRegister<T> res = op(reg1, reg2);
        std::vector<bool> reference(in1.size());

        for (std::size_t i = 0; i < in1.size(); i++)
            reference[i] = op(in1[i], in2[i]);

        REQUIRE(isRegCmpValid(res, reference));
    }

    template <typename BitOperation>
    static auto floatingPointBitOp(BitOperation&& bOp)
    {
        auto bOpRes = [&bOp](auto&& lhs, auto&& rhs)
        {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            static_assert(std::is_same_v<T, U>);

            if constexpr (std::is_floating_point_v<T>)
            {
                using ResType = typename std::conditional<std::is_same_v<T, float>, uint32_t, uint64_t>::type;

                auto* lhs_ = reinterpret_cast<const ResType*>(&lhs);
                auto* rhs_ = reinterpret_cast<const ResType*>(&rhs);

                ResType res = bOp(*lhs_, *rhs_);

                return *reinterpret_cast<T*>(&res);
            }
            else
            {
                // This is a SymdRegister.
                return bOp(rhs, lhs);
            }
        };

        return bOpRes;
    }

    template <typename BitOperation>
    static decltype(auto) floatingPointBitUnaryOp(BitOperation&& uBOp)
    {
        auto uBOpRes = [&uBOp](auto&& arg)
        {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_floating_point_v<T>)
            {
                using ResType = typename std::conditional<std::is_same_v<T, float>, uint32_t, uint64_t>::type;

                auto* arg_ = reinterpret_cast<const ResType*>(&arg);
                ResType res = uBOp(*arg_);

                return *reinterpret_cast<T*>(&res);
            }
            else
            {
                // This is a SymdRegister.
                return uBOp(arg);
            }
        };

        return uBOpRes;
    }

    /// <summary>
    /// Fills input vector with random floats in range 0-255
    /// </summary>
    static void randomizeData(std::vector<float>& data)
    {
        std::default_random_engine generator;
        std::uniform_real_distribution<float> distribution(0.f, 255.f);

        for (auto& x : data)
            x = distribution(generator);
    }

    /// <summary>
    /// Fills input vector with random ints in range 0-255
    /// </summary>
    static void randomizeData(std::vector<int>& data)
    {
        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(0, 255);

        for (auto& x : data)
            x = distribution(generator);
    }

    template <typename T>
    static void requireEqual(const T* data, const std::vector<T>& ref)
    {
        for (size_t i = 0; i < ref.size(); i++)
            REQUIRE(data[i] == ref[i]);
    }

    template <typename T>
    static void requireEqual(const std::vector<T>& data, const std::vector<T>& ref)
    {
        REQUIRE(data.size() == ref.size());

        requireEqual(data.data(), ref);
    }

    template <typename T>
    static void requireNear(const std::vector<T>& data, const std::vector<T>& ref, T eps)
    {
        REQUIRE(data.size() == ref.size());

        for (size_t i = 0; i < ref.size(); i++)
            REQUIRE(std::abs(data[i] - ref[i]) < eps);
    }

    template <typename StencilView, typename DataType>
    auto conv3x3_Kernel(const StencilView& sv, const DataType* kernel)
    {
        return
            sv(-1, -1) * kernel[0] + sv(-1, 0) * kernel[1] + sv(-1, 1) * kernel[2] +
            sv(0, -1) * kernel[3] + sv(0, 0) * kernel[4] + sv(0, 1) * kernel[5] +
            sv(1, -1) * kernel[6] + sv(1, 0) * kernel[7] + sv(1, 1) * kernel[8];
    }

    template <typename StencilView, typename DataType>
    auto conv5x5_Kernel(const StencilView& sv, const DataType* kernel)
    {
        return
            sv(-2, -2) * kernel[0] + sv(-2, -1) * kernel[1] + sv(-2, 0) * kernel[2] + sv(-2, 1) * kernel[3] + sv(-2, 2) * kernel[4] +
            sv(-1, -2) * kernel[5] + sv(-1, -1) * kernel[6] + sv(-1, 0) * kernel[7] + sv(-1, 1) * kernel[8] + sv(-1, 2) * kernel[9] +
            sv(0, -2) * kernel[10] + sv(0, -1) * kernel[11] + sv(0, 0) * kernel[12] + sv(0, 1) * kernel[13] + sv(0, 2) * kernel[14] +
            sv(1, -2) * kernel[15] + sv(1, -1) * kernel[16] + sv(1, 0) * kernel[17] + sv(1, 1) * kernel[18] + sv(1, 2) * kernel[19] +
            sv(2, -2) * kernel[20] + sv(2, -1) * kernel[21] + sv(2, 0) * kernel[22] + sv(2, 1) * kernel[23] + sv(2, 2) * kernel[24];
    }

}