# Symd
C++ header only template library designed to make it easier to write high-performance (vector, multi-threaded), image and data processing code on modern machines. It automatically generates vector (SIMD, SSE, AVX, NEON) code. bfloat16 is supported.

## Requirements

C++17 is a requirement.

### Compiler

Symd can be used with, g++ and Clang. Visual Studio should be supported but we do not test it for now.

#### Compiling tests on Ubuntu - g++

To can compile tests with g++ go to tests folder and run:

```
make
```

#### Compiling tests with Clang

If you have Clang installed just got to tests folder and run ```make clang``.

#### Compiling tests with Visual Studio

TBD

#### TBB support

Symd can also be used with [Intel's TBB](https://software.intel.com/content/www/us/en/develop/tools/oneapi/components/onetbb.html) support.
To main requirement here is to have TBB installed on your system, which can be achieved in various ways depending on the platform you are on.
The next one is setting the TBB switch compile time flag before including the symd library iteself:

```cpp
#define SYMD_USE_TBB 1
#include "symd.h"
```

##### Building tests with TBB on linux

With gcc:
```
g++ test_symd.cpp test_symd_register.cpp test_stencil_borders.cpp -std=c++17 -ltbb -march=ivybridge -O3 -o test_symd
```

With clang:
```
clang++ test_symd.cpp test_symd_register.cpp test_stencil_borders.cpp -std=c++17 -ltbb -mavx -O3 -o test_symd
```

##### Building tests with TBB on windows
The easiest way to set up TBB on windows is by using the [vcpkg](https://github.com/microsoft/vcpkg) package manager, and then installing TBB library with it.
This way no further changes to the build system need to be made in order to run the tests successfully.

### CPU

 * Intel or AMD x64 CPU with [AVX2 support](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions). Roughly CPUs from 2011 and later.
 * ARM based CPUs with Neon support - still in beta phase.

## Usage
Symd is a header-only library. To use Symd in your project you need to:

 * Copy LibSymd/include folder to your project
 * Include symd.h
 * And you are ready to go

### Simple map example:

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

Operations on input vetor are performed using SSE, AVX (or Neon TBD).

### Can I use 2D inputs?

Yes. You need to wrap your data with 2D data_view. Example:

```cpp
size_t width = 640;
size_t height = 480;

std::vector<float> input1(width * height);
std::vector<float> input2(input1.size());
std::vector<float> output(input1.size());

auto input1_2d = symd::views::data_view_2d(input1.data(), width, height, width);
auto input2_2d = symd::views::data_view_2d(input2.data(), width, height, width);
auto output_2d = symd::views::data_view_2d(output.data(), width, height, width);

symd::map(output_2d, [&](auto a, auto b) { return a + b; }, input1_2d, input2_2d);
```

symd::views::data_view is non-owning view of underlying data.

### Can I use my own tensor or matrix class as input or output to Symd?

Chances are that you will be using your own tensor/matrix/vector class or some third party class for storing data which are not natively supported by Symd (eg OpenCV matrix). 
Using such classes as inputs or outputs with Symd is possible. You need to overload methods for getting size of data and accessing elements. Example:

```cpp
namespace symd::__internal__
{
    template <typename T>
    Dimensions getShape(const MyMatrix<T>& myMatrix)
    {
        return Dimensions({ myMatrix.height(), myMatrix.width() });
    }

    template <typename T>
    Dimensions getPitch(const MyMatrix<T>& myMatrix)
    {
        return  Dimensions({ myMatrix.pitch(), 1 });
    }

    template <typename T>
    T* getDataPtr(MyMatrix<T>& myMatrix, const Dimensions& coords)
    {
        return &myMatrix(coords[1], coords[0]);
    }

    template <typename T>
    const T* getDataPtr(const MyMatrix<T>& myMatrix, const Dimensions& coords)
    {
        return &myMatrix(coords[1], coords[0]);
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

auto input_2d = symd::views::data_view_2d(input.data(), width, height, width);
auto output_2d = symd::views::data_view_2d(output.data(), width, height, width);

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
