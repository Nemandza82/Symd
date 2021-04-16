// SimpleTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "../LibSymd/include/symd.h"


int main()
{
    std::vector<int> input = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    std::vector<int> output(input.size());

    symd::map_single_core(output, [](auto x) { return x * 2; }, input);

    for (auto x : output)
        std::cout << x << ", ";

    return 0;
}
