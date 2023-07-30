#pragma once
#include <array>
#include <vector>
#include <cassert>
#include <algorithm>


namespace symd
{
    /// <summary>
    /// Struct representing shape of input data.
    /// </summary>
    class Dimensions
    {
        static constexpr int MAX_DIMS = 5;

        std::array<int64_t, MAX_DIMS> _dims;
        int _ndims;

    public:

        Dimensions()
        {
            _dims[0] = 1;
            _ndims = 1;
        }

        Dimensions(const std::vector<int64_t>& dims)
        {
            assert(dims.size() <= MAX_DIMS);
            assert(dims.size() > 0);

            _ndims = dims.size();

            for (int i=0; i<_ndims; i++)
                _dims[i] = dims[i];
        }

        int64_t operator[](int ind) const
        {
            if (ind < 0)
                ind += _ndims;

            return _dims[ind];
        }



        void set_ith_dim(int i, int64_t new_val)
        {
            _dims[i] = new_val;
        }

        Dimensions with_i(int i, int64_t new_val) const
        {
            Dimensions res = *this;
            res._dims[i] = new_val;
            return res;
        }

        Dimensions add(int64_t d0) const
        {
            assert(_ndims == 1);
            Dimensions res = *this;

            res._dims[0] += d0;
            return res;
        }

        Dimensions add(int64_t d0, int64_t d1) const
        {
            assert(_ndims == 2);
            Dimensions res = *this;

            res._dims[0] += d0;
            res._dims[1] += d1;
            
            return res;
        }

        Dimensions add(int64_t d0, int64_t d1, int64_t d2) const
        {
            assert(_ndims == 3);
            Dimensions res = *this;

            res._dims[0] += d0;
            res._dims[1] += d1;
            res._dims[2] += d2;
            
            return res;
        }

        Dimensions add(int64_t d0, int64_t d1, int64_t d2, int64_t d3) const
        {
            assert(_ndims == 4);
            Dimensions res = *this;

            res._dims[0] += d0;
            res._dims[1] += d1;
            res._dims[2] += d2;
            res._dims[3] += d3;
            
            return res;
        }

        Dimensions add(int64_t d0, int64_t d1, int64_t d2, int64_t d3, int64_t d4) const
        {
            assert(_ndims == 5);
            Dimensions res = *this;

            res._dims[0] += d0;
            res._dims[1] += d1;
            res._dims[2] += d2;
            res._dims[3] += d3;
            res._dims[4] += d4;
            
            return res;
        }

        Dimensions native_pitch() const
        {
            Dimensions result;
            result._ndims = this->_ndims;

            result._dims[_ndims - 1] = 1;

            for (int i = _ndims - 2; i >= 0; i--)
                result._dims[i] = result._dims[i + 1] * _dims[i + 1];

            return result;
        }

        Dimensions zeros_like() const
        {
            Dimensions result;
            result._ndims = this->_ndims;

            for (int i = 0; i < _ndims; i++)
                result._dims[i] = 0;

            return result;
        }

        Dimensions native_border() const
        {
            return this->zeros_like();
        }

        Dimensions operator+(const Dimensions& other) const
        {
            assert(this->_ndims == other._ndims);
            Dimensions result = *this;

            for (int i = 0; i < this->_ndims; i++)
                result._dims[i] += other._dims[i];

            return result;
        }

        Dimensions operator-(const Dimensions& other) const
        {
            assert(this->_ndims == other._ndims);
            Dimensions result = *this;

            for (int i = 0; i < this->_ndims; i++)
                result._dims[i] -= other._dims[i];

            return result;
        }

        Dimensions operator+(int64_t other) const
        {
            Dimensions result = *this;

            for (int i = 0; i < this->_ndims; i++)
                result._dims[i] += other;
            
            return result;
        }

        Dimensions operator-(int64_t other) const
        {
            Dimensions result = *this;

            for (int i = 0; i < this->_ndims; i++)
                result._dims[i] = result._dims[i] - other;
            
            return result;
        }

        Dimensions eltwise_max(const Dimensions& other)
        {
            Dimensions result = *this;

            for (int i = 0; i < this->_ndims; i++)
                result._dims[i] = std::max(result._dims[i], other._dims[i]);
            
            return result;
        }

        Dimensions eltwise_min(const Dimensions& other)
        {
            Dimensions result = *this;

            for (int i = 0; i < this->_ndims; i++)
                result._dims[i] = std::min(result._dims[i], other._dims[i]);
            
            return result;
        }

        bool are_outside(const Dimensions& coords) const
        {
            assert(this->_ndims == coords._ndims);

            for (int i = 0; i < coords._ndims; i++)
            {
                if (coords[i] < 0)
                    return true;

                if (coords[i] >= _dims[i])
                    return true;
            }

            return false;
        }

        Dimensions mirrorCoords(const Dimensions& coords) const
        {
            assert(this->_ndims == coords._ndims);
            Dimensions result = coords;

            for (int i = 0; i < coords._ndims; i++)
            {
                if (coords[i] < 0)
                    result._dims[i] = -coords[i];
                else if (coords[i] >= _dims[i] - 1)
                    result._dims[i] = 2 * (_dims[i] - 1) - std::abs(coords[i]);
            }

            return result;
        }

        Dimensions replicateCoords(const Dimensions& coords) const
        {
            assert(this->_ndims == coords._ndims);
            Dimensions result = coords;

            for (int i = 0; i < coords._ndims; i++)
            {
                if (coords[i] < 0)
                    result._dims[i] = 0;
                else if (coords[i] > _dims[i] - 1)
                    result._dims[i] = _dims[i] - 1;
            }

            return result;
        }

        // Only the border next to anchor is replicated, rest is mirrored.
        Dimensions replicateMirrorCoords(const Dimensions& coords) const
        {
            assert(this->_ndims == coords._ndims);

            Dimensions result = coords;

            for (int i = 0; i < coords._ndims; i++)
            {
                if (coords[i] < 0)
                {
                    if (coords[i] < -1)
                        result._dims[i] = std::abs(coords[i]) - 1;
                    else
                        result._dims[i] = 0;
                }
                else if (coords[i] > _dims[i] - 1)
                {
                    if (coords[i] > _dims[i])
                        result._dims[i] = 2 * (_dims[i] - 1) - std::abs(coords[i]) + 1;
                    else
                        result._dims[i] = _dims[i] - 1;
                }
            }

            return result;
        }

        int num_dims() const
        {
            return _ndims;
        }

        int64_t num_elements() const
        {
            int64_t res = 1;

            for (int i = 0; i < this->_ndims; i++)
                res *= _dims[i];

            return res;
        }
    };
}
