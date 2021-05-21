# Symd
C++ header only template library designed to make it easier to write high-performance (vector, multi-threaded), image and data processing code on modern machines. It automatically generates vector (SIMD, SSE, AVX, NEON) code.

## Requirements

C++17 is a requirement.

### Compiler

Symd can be used with Visual Studio, g++, and Clang.

#### Compiling on Windows

For development and testing we use Visual Studio 2019 (16.9.4), with /std:c++17 setting. 
We ship Visual Studio solution file, so just load it.

#### Compiling on Ubuntu - g++

We tested with gcc version 9.3.0. To can compile tests with g++ run:

```
g++ test_symd.cpp test_symd_register.cpp -std=c++17 -march=ivybridge -O3 -o test_symd
```

#### Compiling on Ubuntu - Clang

We tested with clang version 10.0.0. To can compile tests with Clang run:

```
clang++ test_symd.cpp test_symd_register.cpp -std=c++17 -mavx -O3 -o test_symd
```

### CPU

 * Intel or AMD x64 CPU with [AVX support](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions). Roughly CPUs from 2011 and later.
 * ARM based CPUs with Neon support - still in beta phase.

## Usage
Symd is a header-only library. To use Symd in your project you need to:

 * Copy LibSymd/include folder to your project
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

Operations on input vetor are performed using SSE, AVX (or Neon).

### Can I use 2D inputs?

Yes. You need to wrap your data with 2D data_view. Example:

```cpp
size_t width = 640;
size_t height = 480;

std::vector<float> input1(width * height);
std::vector<float> input2(input1.size());
std::vector<float> output(input1.size());

symd::views::data_view<float, 2> input1_2d(input1.data(), width, height, width);
symd::views::data_view<float, 2> input2_2d(input2.data(), width, height, width);
symd::views::data_view<float, 2> output_2d(output.data(), width, height, width);

symd::map(output_2d, [&](auto a, auto b) { return a + b; }, input1_2d, input2_2d);
```

symd::views::data_view is non-owning view of underlying data.

### Can I use my own matrix class as input or output to Symd?

Chances are that you will be using your own matrix/vector class or some third party class for storing data which are not natively supported by Symd (eg OpenCV matrix). 
Using such classes as inputs or outputs with Symd is possible. You need to overload methods for getting size of data and accessing elements before including Symd. Example:

```cpp
namespace symd::__internal__
{
    template <typename T>
    size_t getWidth(const MyMatrix<T>& myMatrix)
    {
        return myMatrix.width();
    }

    template <typename T>
    size_t getHeight(const MyMatrix<T>& myMatrix)
    {
        return myMatrix.height();
    }

    template <typename T>
    size_t getPitch(const MyMatrix<T>& myMatrix)
    {
        return myMatrix.pitch();
    }

    template <typename T>
    T* getDataPtr(MyMatrix<T>& myMatrix, size_t row, size_t col)
    {
        return &myMatrix(row, col);
    }

    template <typename T>
    const T* getDataPtr(const MyMatrix<T>& myMatrix, size_t row, size_t col)
    {
        return &myMatrix(row, col);
    }
}

#include "../LibSymd/include/symd.h"


void myMatrixExample()
{
    MyMatrix<float> A(1920, 1080);
    MyMatrix<float> B(1920, 1080);

    MyMatrix<float> res(1920, 1080);

    symd::map(res, [](auto a, auto b) { return a + b;  }, A, B);
}
```


### How can I access nearby elements in the Symd kernel (implement convolution)?

To access nearby elements in Symd kernel, you need to use stencil view (symd::views::stencil). Example:

```cpp
size_t width = 640;
size_t height = 480;

std::vector<float> input(width * height);
std::vector<float> output(input.size());

symd::views::data_view<float, 2> input_2d(input.data(), width, height, width);
symd::views::data_view<float, 2> output_2d(output.data(), width, height, width);

// Calculate image gradient. We also need 2D stencil view.
symd::map(output_2d, [&](const auto& sv) { return sv(0, 1) - sv(0, -1); },
	symd::views::stencil(input_2d, 3, 3));
```


### How can I perform reduction?

You need to create reduce_view and specify reduce operation. Example:

```cpp
std::vector<float> input = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18 };

// Reduce operation is summing
auto sum = symd::views::reduce_view(input.size(), 1, 0.0f, [](auto x, auto y)
    {
        return x + y;
    });
```

After that you map your inputs to reduce_view. That enables you to do some processing of input data prior to reducing.

```cpp
symd::map(sum, [](auto x) { return x * 2; }, input);
```

Getting the result of reduction:

```cpp
float result = sum.getResult();
```

## Maintainers

 * [Nemandza82](https://github.com/Nemandza82)
 * [0508994](https://github.com/0508994)

## License

Licensed under the [MIT License](LICENSE).
