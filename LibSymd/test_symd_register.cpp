#include "catch.h"
#include "include/internal/symd_register.h"

#pragma warning( push )
#pragma warning( disable : 26444 )

using namespace symd::__internal__;

namespace tests
{
    /////////////////////////////////////////////////////////////////////////////////////////
    /// Helper functions
    /////////////////////////////////////////////////////////////////////////////////////////

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
                // This is a regular value (floating point values don't support bitwise operations).
                union
                {
                    T f;
                    typename std::conditional<std::is_same_v<T, float>, uint32_t, uint64_t>::type u;
                } lhs_, rhs_, res;

                lhs_.f = lhs;
                rhs_.f = rhs;

                res.u = bOp(lhs_.u, rhs_.u);
                return res.f;
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
                // This is a regular value (floating point values don't support bitwise operations).
                union
                {
                    T f;
                    typename std::conditional<std::is_same_v<T, float>, uint32_t, uint64_t>::type u;
                } arg_, res;

                arg_.f = arg;

                res.u = uBOp(arg_.u);
                return res.f;
            }
            else
            {
                // This is a SymdRegister.
                return uBOp(arg);
            }
        };

        return uBOpRes;
    }

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
            return std::make_pair(std::cref(inData1F), std::cref(inData2F));
        }
        else
        {
            return std::make_pair(std::cref(inData1D), std::cref(inData2D));
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
            return std::make_pair(std::cref(inData1i), std::cref(inData2i));
        }
        else
        {
            return std::make_pair(std::cref(inData1b), std::cref(inData2b));
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

#pragma warning( pop )