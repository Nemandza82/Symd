#include <iostream>
#include <stdio.h>
#include <vector>


template<typename T>
class MyMatrix
{
    std::vector<T> _data;
    size_t _width;
    size_t _height;

public:

    MyMatrix(size_t width, size_t height)
        : _data(width * height)
    {
        _width = width;
        _height = height;
    }

    size_t width() const
    {
        return _width;
    }

    size_t height() const
    {
        return _height;
    }

    size_t pitch() const
    {
        return _width;
    }

    T& operator()(size_t row, size_t col)
    {
        return _data[row * _width + col];
    }

    const T& operator()(size_t row, size_t col) const
    {
        return _data[row * _width + col];
    }
};


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
        return &myMatrix(row, + col);
    }

    template <typename T>
    const T* getDataPtr(const MyMatrix<T>& myMatrix, size_t row, size_t col)
    {
        return &myMatrix(row, +col);
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
