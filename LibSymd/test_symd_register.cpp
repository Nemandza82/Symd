#include "test_helpers.h"

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

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Test int operations
    /////////////////////////////////////////////////////////////////////////////////////////

    static std::vector<int> inData1i{ 1, 2, 3, 4, 4, 6, 7, 8 };
    static std::vector<int> inData2i{ 8, 7, 6, 4, 4, 3, 2, 1 };

    static std::vector<unsigned char> inData1b{ 1, 2, 3, 4, 4, 6, 7, 8 };
    static std::vector<unsigned char> inData2b{ 8, 7, 6, 4, 4, 3, 2, 1 };

    template <typename IT>
    std::pair<const std::vector<IT>, const std::vector<IT>> getIntTestData()
    {
        if constexpr (std::is_same_v<IT, int>)
        {
            return std::make_pair(inData1i, inData2i);
        }
        else
        {
            return std::make_pair(inData1b, inData2b);
        }
    }

    TEMPLATE_TEST_CASE("Int addition", "[integer][operators]", int)
    {
        const auto [inData1, inData2] = getIntTestData<TestType>();
        checkOperationResult(inData1, std::plus(), inData2);
        checkOperationResult(inData2, std::plus(), inData1);
    }


    TEMPLATE_TEST_CASE("Int substraction", "[integer][operators]", int)
    {
        const auto [inData1, inData2] = getIntTestData<TestType>();

        checkOperationResult(inData1, std::minus(), inData2);
        checkOperationResult(inData2, std::minus(), inData1);
    }


    TEMPLATE_TEST_CASE("Int multiplication", "[integer][operators]", int)
    {
        const auto [inData1, inData2] = getIntTestData<TestType>();

        checkOperationResult(inData1, std::multiplies(), inData2);
        checkOperationResult(inData2, std::multiplies(), inData1);
    }


    TEMPLATE_TEST_CASE("Int division", "[integer][operators]", int, unsigned char)
    {
        // Division not supported for int. Convert to other type
        //checkOperationResult(inData1, std::divides(), inData2);
    }

    TEMPLATE_TEST_CASE("Int bit and", "[integer][operators]", int, unsigned char)
    {
        const auto [inData1, inData2] = getIntTestData<TestType>();

        checkOperationResult(inData1, std::bit_and(), inData2);
        checkOperationResult(inData2, std::bit_and(), inData1);
    }


    TEMPLATE_TEST_CASE("Int bit or", "[integer][operators]", int, unsigned char)
    {
        const auto [inData1, inData2] = getIntTestData<TestType>();

        checkOperationResult(inData1, std::bit_or(), inData2);
        checkOperationResult(inData2, std::bit_or(), inData1);
    }


    TEMPLATE_TEST_CASE("Int bit xor", "[integer][operators]", int, unsigned char)
    {
        const auto [inData1, inData2] = getIntTestData<TestType>();

        checkOperationResult(inData1, std::bit_xor(), inData2);
        checkOperationResult(inData2, std::bit_xor(), inData1);
    }


    TEMPLATE_TEST_CASE("Int bit not", "[integer][operators]", int, unsigned char)
    {
        const auto [inData1, inData2] = getIntTestData<TestType>();

        checkUnaryOperationResult(std::bit_not(), inData1);
        checkUnaryOperationResult(std::bit_not(), inData2);
    }


    TEMPLATE_TEST_CASE("Int cmp equal", "[integer][operators]", int)
    {
        const auto [inData1, inData2] = getIntTestData<TestType>();

        checkCmpOperationResult(inData1, std::equal_to(), inData2);
        checkCmpOperationResult(inData2, std::equal_to(), inData1);
    }

    TEMPLATE_TEST_CASE("Int cmp not equal", "[integer][operators]", int)
    {
        const auto [inData1, inData2] = getIntTestData<TestType>();

        checkCmpOperationResult(inData1, std::not_equal_to(), inData2);
        checkCmpOperationResult(inData2, std::not_equal_to(), inData1);
    }


    TEMPLATE_TEST_CASE("Int cmp greater equal", "[integer][operators]", int)
    {
        const auto [inData1, inData2] = getIntTestData<TestType>();

        checkCmpOperationResult(inData1, std::greater_equal(), inData2);
        checkCmpOperationResult(inData2, std::greater_equal(), inData1);
    }

    TEMPLATE_TEST_CASE("Int cmp less equal", "[integer][operators]", int)
    {
        const auto [inData1, inData2] = getIntTestData<TestType>();

        checkCmpOperationResult(inData1, std::less_equal(), inData2);
        checkCmpOperationResult(inData2, std::less_equal(), inData1);
    }


    TEMPLATE_TEST_CASE("Int cmp greater", "[integer][operators]", int)
    {
        const auto [inData1, inData2] = getIntTestData<TestType>();

        checkCmpOperationResult(inData1, std::greater(), inData2);
        checkCmpOperationResult(inData2, std::greater(), inData1);
    }


    TEMPLATE_TEST_CASE("Int cmp less", "[integer][operators]", int)
    {
        const auto [inData1, inData2] = getIntTestData<TestType>();

        checkCmpOperationResult(inData1, std::less(), inData2);
        checkCmpOperationResult(inData2, std::less(), inData1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// Test uint specific operations
    /////////////////////////////////////////////////////////////////////////////////////////

    TEST_CASE("Unisgned char addition")
    {
        std::vector<unsigned char> inData1bOf = { 255, 254, 253, 252, 251, 250, 249, 248 };
        std::vector<unsigned char> inData2bOf = { 1, 2, 3, 4, 5, 6, 7, 8 };

        auto ucPlusSat = [](auto&& lhs, auto&& rhs)
        {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            static_assert(std::is_same_v<T, U>);

            if constexpr (std::is_same_v<T, unsigned char>)
            {
                // https://stackoverflow.com/questions/33481295/saturating-subtract-add-for-unsigned-bytes
                unsigned char res = lhs + rhs;
                res |= -(res < lhs);

                return res;
            }
            else
            {
                return lhs + rhs;
            }
        };

        checkOperationResult(inData1bOf, ucPlusSat, inData2bOf);
        checkOperationResult(inData2bOf, ucPlusSat, inData1bOf);
    }


    TEST_CASE("Unisgned char substraction")
    {
        auto ucMinusSat = [](auto&& lhs, auto&& rhs)
        {
            using T = std::decay_t<decltype(lhs)>;
            using U = std::decay_t<decltype(rhs)>;
            static_assert(std::is_same_v<T, U>);

            if constexpr (std::is_same_v<T, unsigned char>)
            {
                unsigned char res = lhs - rhs;
                res &= -(res <= lhs);

                return res;
            }
            else
            {
                return lhs - rhs;
            }
        };

        checkOperationResult(inData1b, ucMinusSat, inData2b);
        checkOperationResult(inData2b, ucMinusSat, inData1b);
    }
}