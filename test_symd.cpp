#include <iostream>
#include "lib/symd.h"

namespace symd
{
    namespace internal
    {
        void test()
        {

            SymdRegister<float> x(10);

            auto res = 10 + x;

            //auto v = x.convert_to<int>();

            SymdRegister<int> y(10);
            SymdRegister<int> z(5);

            y = y + z;

            y = std::min(y, z);

            return;
        }
    }

    void test_map()
    {
        std::vector<int> input = {1, 2, 3, 4, 5, 6, 7, 8};
        std::vector<int> output(input.size());

        symd::map_single_core(output, [](auto x) { return x * 2; }, input);

        input.size();
    }
}

int main()
{
    symd::internal::test();

    symd::test_map();
}
