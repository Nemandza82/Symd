#pragma once
#include "../test_helpers.h"


namespace tests
{
    /////////////////////////////////////////////////////////////////////////////////////////
    /// Test float operations
    /////////////////////////////////////////////////////////////////////////////////////////

    static std::vector<float> inData1F{ 1, 2, 3, 4, 4, 6, 7, 8 };
    static std::vector<float> inData2F{ 8, 7, 6, 4, 4, 3, 2, 1 };

    static std::vector<double> inData1D{ 1, 2, 3, 4, 4, 6, 7, 8 };
    static std::vector<double> inData2D{ 8, 7, 6, 4, 4, 3, 2, 1 };

    template <typename FT>
    std::pair<const std::vector<FT>, const std::vector<FT>> getFpTestData()
    {
        if constexpr (std::is_same_v<FT, float>) 
        {
            return std::make_pair(inData1F, inData2F);
        }
        else
        {
            return std::make_pair(inData1D, inData2D);
        }
    }

    TEMPLATE_TEST_CASE("Float addition", "[float][operators]", float, double)
    {
        const auto [inData1, inData2] = getFpTestData<TestType>();

        checkOperationResult(inData1, std::plus(), inData2);
        checkOperationResult(inData2, std::plus(), inData1);

        SymdRegister<TestType> reg1(inData1.data());
        SymdRegister<TestType> res;

        auto reg1Plus10 = applyOpToVector(inData1, std::plus(), (TestType)10.0);

        res = reg1 + 10;
        REQUIRE(isRegEqualToData(res, reg1Plus10));

        res = 10 + reg1;
        REQUIRE(isRegEqualToData(res, reg1Plus10));

        res = reg1;
    }


    TEMPLATE_TEST_CASE("Float substraction", "[float][operators]", float, double)
    {
        const auto [inData1, inData2] = getFpTestData<TestType>();

        checkOperationResult(inData1, std::minus(), inData2);
        checkOperationResult(inData2, std::minus(), inData1);
    }


    TEMPLATE_TEST_CASE("Float multiplication", "[float][operators]", float, double)
    {
        const auto [inData1, inData2] = getFpTestData<TestType>();

        checkOperationResult(inData1, std::multiplies(), inData2);
        checkOperationResult(inData2, std::multiplies(), inData1);
    }


    TEMPLATE_TEST_CASE("Float division", "[float][operators]", float, double)
    {
        const auto [inData1, inData2] = getFpTestData<TestType>();

        checkOperationResult(inData1, std::divides(), inData2);
        checkOperationResult(inData2, std::divides(), inData1);
    }


    TEMPLATE_TEST_CASE("Float bit and", "[float][operators]", float, double)
    {
        const auto [inData1, inData2] = getFpTestData<TestType>();

        checkOperationResult(inData1, floatingPointBitOp(std::bit_and()), inData2);
        checkOperationResult(inData2, floatingPointBitOp(std::bit_and()), inData1);
    }


    TEMPLATE_TEST_CASE("Float bit or", "[float][operators]", float, double)
    {
        const auto [inData1, inData2] = getFpTestData<TestType>();

        checkOperationResult(inData1, floatingPointBitOp(std::bit_or()), inData2);
        checkOperationResult(inData2, floatingPointBitOp(std::bit_or()), inData1);
    }


    TEMPLATE_TEST_CASE("Float bit xor", "[float][operators]", float, double)
    {
        const auto [inData1, inData2] = getFpTestData<TestType>();

        checkOperationResult(inData1, floatingPointBitOp(std::bit_xor()), inData2);
        checkOperationResult(inData2, floatingPointBitOp(std::bit_xor()), inData1);
    }


    TEMPLATE_TEST_CASE("Float bit not", "[float][operators]", float, double)
    {
        const auto [inData1, inData2] = getFpTestData<TestType>();

        checkUnaryOperationResult(floatingPointBitUnaryOp(std::bit_not()), inData1);
        checkUnaryOperationResult(floatingPointBitUnaryOp(std::bit_not()), inData2);
    }


    TEMPLATE_TEST_CASE("Float cmp equal", "[float][operators]", float, double)
    {
        const auto [inData1, inData2] = getFpTestData<TestType>();

        checkCmpOperationResult(inData1, std::equal_to(), inData2);
        checkCmpOperationResult(inData2, std::equal_to(), inData1);
    }

    TEMPLATE_TEST_CASE("Float cmp not equal", "[float][operators]", float, double)
    {
        const auto [inData1, inData2] = getFpTestData<TestType>();

        checkCmpOperationResult(inData1, std::not_equal_to(), inData2);
        checkCmpOperationResult(inData2, std::not_equal_to(), inData1);
    }


    TEMPLATE_TEST_CASE("Float cmp greater equal", "[float][operators]", float, double)
    {
        const auto [inData1, inData2] = getFpTestData<TestType>();

        checkCmpOperationResult(inData1, std::greater_equal(), inData2);
        checkCmpOperationResult(inData2, std::greater_equal(), inData1);
    }


    TEMPLATE_TEST_CASE("Float cmp less equal", "[float][operators]", float, double)
    {
        const auto [inData1, inData2] = getFpTestData<TestType>();

        checkCmpOperationResult(inData1, std::less_equal(), inData2);
        checkCmpOperationResult(inData2, std::less_equal(), inData1);
    }

    TEMPLATE_TEST_CASE("Float cmp greater", "[float][operators]", float, double)
    {
        const auto [inData1, inData2] = getFpTestData<TestType>();

        checkCmpOperationResult(inData1, std::greater(), inData2);
        checkCmpOperationResult(inData2, std::greater(), inData1);
    }


    TEMPLATE_TEST_CASE("Float cmp less", "[float][operators]", float, double)
    {
        const auto [inData1, inData2] = getFpTestData<TestType>();

        checkCmpOperationResult(inData1, std::less(), inData2);
        checkCmpOperationResult(inData2, std::less(), inData1);
    }
}