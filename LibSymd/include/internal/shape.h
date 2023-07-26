#pragma once
#include <array>

namespace symd::__internal__
{
    /// <summary>
    /// Struct representing shape of input data.
    /// </summary>
    class Shape
    {
        std::array<size_t, 7> _dims;
        int ndims;

    public:

        /// <summary>
        /// Constructs one dimensional shape.
        /// </summary>
        Shape(size_t length)
        {

        }

        T operator[](size_t ind) const
        {
            return _ptrToData[ind];
        }
    }
}
