#pragma once
#include "../test_helpers.h"


namespace tests
{
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