#pragma once
#include <array>
#include <vector>
#include <cassert>


namespace symd
{
    /// <summary>
    /// Struct representing shape of input data.
    /// </summary>
    class Shape
    {
        static constexpr size_t MAX_DIMS = 7;

        std::array<size_t, MAX_DIMS> _dims;
        int ndims;

    public:

        Shape(std::vector<size_t> dims)
        {
            assert(dims.size() <= MAX_DIMS);
            ndims = dims.size();

            for (auto i=0; i<ndims; i++)
                _dims[i] = dims[i];
        }

        size_t operator[](int ind) const
        {
            if (ind < 0)
                ind += ndims;

            return _dims[ind];
        }
    };
}
