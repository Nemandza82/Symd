# Symd
C++ header only template library designed to make it easier to write high-performance (vector, multi-threaded), image and data processing code on modern machines. It automatically generates vector (SIMD, SSE, AVX, NEON) code.

# Usage
 * Just copy headers from lib folder to your project
 * Include symd.h
 * And you are ready to go

Simple map example:

```cpp
#define CATCH_CONFIG_MAIN
#include "tests/catch.h"
#include <iostream>
#include "lib/symd.h"

TEST_CASE("Mapping 1")
{
    std::vector<int> input = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> output(input.size());

    symd::map_single_core(output, [](auto x) { return x * 2; }, input);

    requireEqual(output, { 2, 4, 6, 8, 10, 12, 14, 16, 18 });
}
```