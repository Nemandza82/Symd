#include "tests/catch.h"
#include "lib/symd_register.h"

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


    template <typename T, typename Operation>
    std::vector<T> applyOpToVector(const std::vector<T>& in, Operation op, T x)
    {
        std::vector<T> reference(in.size());

        for (size_t i = 0; i < in.size(); i++)
            reference[i] = op(in[i], x);

        return reference;
    }


    template <typename T, typename Operation>
    std::vector<T> applyOpToVector(T x, Operation op, const std::vector<T>& in)
    {
        std::vector<T> reference(in.size());

        for (size_t i = 0; i < in.size(); i++)
            reference[i] = op(x, in[i]);

        return reference;
    }


    template <typename T, typename Operation>
    std::vector<T> applyOpToVector(const std::vector<T>& in1, Operation op, const std::vector<T>& in2)
    {
        std::vector<T> reference(in1.size());

        for (size_t i = 0; i < in1.size(); i++)
            reference[i] = op(in1[i], in2[i]);

        return reference;
    }


    template <typename T, typename Operation>
    static void checkOperationResult(const std::vector<T>& in1, Operation op, const std::vector<T>& in2)
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
    static void checkUnaryOperationResult(Operation op, const std::vector<T>& in)
    {
        SymdRegister<T> reg(in.data());
        REQUIRE(isRegEqualToData(reg, in));

        SymdRegister<T> res = op(reg);
        std::vector<T> reference(in.size());

        for (size_t i = 0; i < in.size(); i++)
            reference[i] = op(in[i]);

        REQUIRE(isRegEqualToData(res, reference));
    }


    /////////////////////////////////////////////////////////////////////////////////////////
    /// Test float operations
    /////////////////////////////////////////////////////////////////////////////////////////

    static std::vector<float> inData1F{ 1, 2, 3, 4, 4, 6, 7, 8 };
    static std::vector<float> inData2F{ 8, 7, 6, 4, 4, 3, 2, 1 };


    TEST_CASE("Float addition")
    {
        checkOperationResult(inData1F, std::plus(), inData2F);
        checkOperationResult(inData2F, std::plus(), inData1F);

        SymdRegister<float> reg1(inData1F.data());
        SymdRegister<float> res;

        auto reg1Plus10 = applyOpToVector(inData1F, std::plus(), 10.0f);

        res = reg1 + 10;
        REQUIRE(isRegEqualToData(res, reg1Plus10));

        res = 10 + reg1;
        REQUIRE(isRegEqualToData(res, reg1Plus10));

        res = reg1;
    }


    TEST_CASE("Float substraction")
    {
        checkOperationResult(inData1F, std::minus(), inData2F);
        checkOperationResult(inData2F, std::minus(), inData1F);
    }


    TEST_CASE("Float multiplication")
    {
        checkOperationResult(inData1F, std::multiplies(), inData2F);
        checkOperationResult(inData2F, std::multiplies(), inData1F);
    }


    TEST_CASE("Float division")
    {
        checkOperationResult(inData1F, std::divides(), inData2F);
        checkOperationResult(inData2F, std::divides(), inData1F);
    }


    /////////////////////////////////////////////////////////////////////////////////////////
    /// Test int operations
    /////////////////////////////////////////////////////////////////////////////////////////

    std::vector<int> inData1i{ 1, 2, 3, 4, 4, 6, 7, 8 };
    std::vector<int> inData2i{ 8, 7, 6, 4, 4, 3, 2, 1 };

    TEST_CASE("Int addition")
    {
        checkOperationResult(inData1i, std::plus(), inData2i);
        checkOperationResult(inData2i, std::plus(), inData1i);
    }

    TEST_CASE("Int substraction")
    {
        checkOperationResult(inData1i, std::minus(), inData2i);
        checkOperationResult(inData2i, std::minus(), inData1i);
    }

    TEST_CASE("Int multiplication")
    {
        checkOperationResult(inData1i, std::multiplies(), inData2i);
        checkOperationResult(inData2i, std::multiplies(), inData1i);
    }

    TEST_CASE("Int division")
    {
        // Division not supported for int. Convert to other type
        //checkOperationResult(inData1, std::divides(), inData2);
    }

    TEST_CASE("Int bit and")
    {
        checkOperationResult(inData1i, std::bit_and(), inData2i);
        checkOperationResult(inData2i, std::bit_and(), inData1i);
    }

    TEST_CASE("Int bit or")
    {
        checkOperationResult(inData1i, std::bit_or(), inData2i);
        checkOperationResult(inData2i, std::bit_or(), inData1i);
    }

    TEST_CASE("Int bit xor")
    {
        checkOperationResult(inData1i, std::bit_xor(), inData2i);
        checkOperationResult(inData2i, std::bit_xor(), inData1i);
    }

    TEST_CASE("Int bit not")
    {
        checkUnaryOperationResult(std::bit_not(), inData1i);
        checkUnaryOperationResult(std::bit_not(), inData2i);
    }
}
