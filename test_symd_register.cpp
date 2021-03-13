#include "tests/catch.h"
#include "lib/symd_register.h"

using namespace symd::internal;

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


TEST_CASE("float arithmetic operations")
{
    std::vector<float> inData1{ 1, 2, 3, 4, 4, 6, 7, 8 };
    std::vector<float> inData2{ 8, 7, 6, 4, 4, 3, 2, 1 };

    checkOperationResult(inData1, std::plus(), inData2);
    checkOperationResult(inData1, std::minus(), inData2);
    checkOperationResult(inData1, std::multiplies(), inData2);
    checkOperationResult(inData1, std::divides(), inData2);

    checkOperationResult(inData1, std::equal_to(), inData2);
    //checkOperationResult(inData1, std::bit_or(), inData2);
    //checkOperationResult(inData1, std::bit_xor(), inData2);*/
}

TEST_CASE("int arithmetic operations")
{
    std::vector<int> inData1{ 1, 2, 3, 4, 4, 6, 7, 8 };
    std::vector<int> inData2{ 8, 7, 6, 4, 4, 3, 2, 1 };

    checkOperationResult(inData1, std::plus(), inData2);
    checkOperationResult(inData1, std::minus(), inData2);
    checkOperationResult(inData1, std::multiplies(), inData2);

    // Division not supported for int. Convert to other type
    //checkOperationResult(inData1, std::divides(), inData2);

    checkOperationResult(inData1, std::bit_and(), inData2);
    checkOperationResult(inData1, std::bit_or(), inData2);
    checkOperationResult(inData1, std::bit_xor(), inData2);
    checkUnaryOperationResult(std::bit_not(), inData1);
    checkUnaryOperationResult(std::bit_not(), inData2);
}
