# Symd
C++ header only template library designed to make it easier to write high-performance (vector, multi-threaded), image and data processing code on modern machines. It automatically generates vector (SIMD, SSE, AVX, NEON) code.

# Usage
 * Just copy LibSymd/include folder to your project
 * Include symd.h
 * And you are ready to go

Simple map example:

```cpp
#include <iostream>
#include "../LibSymd/include/symd.h"


void main()
{
    std::vector<int> input = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    std::vector<int> output(input.size());

    symd::map(output, [](auto x) { return x * 2; }, input);

    for (auto x : output)
        std::cout << x << ", ";
}
```

Output:

```
2, 4, 6, 8, 10, 12, 14, 16, 18,
```