#pragma once
#include <vector>
#include <utility>
#include "../dimensions.h"


namespace symd::__internal__
{
    /// <summary>
    /// Inclusive region. Used to help parallel compute.
    /// </summary>
    struct Region
    {
        Dimensions startCoord;
        Dimensions endCoord;

        Region(const Dimensions& sc, const Dimensions& ec)
            : startCoord(sc)
            , endCoord(ec)
        {
        }

        Region(const Dimensions& shape)
        {
            startCoord = shape.zeros_like();
            endCoord = shape - 1;
        }

        /// <summary>
        /// Shape of Region
        /// </summary>
        Dimensions getShape() const
        {
            return endCoord - startCoord + 1;
        }

        /// <summary>
        /// Number of elements in Region
        /// </summary>
        int64_t num_elements() const
        {
            return getShape().num_elements();
        }

        /// <summary>
        /// Splits the Region in two roughly equal parts.
        /// </summary>
        /// <returns>Two disjoint regions with cover the source region.</returns>
        void split(std::vector<Region>& result) const
        {
            if (this->num_elements() < 100000)
            {
                result.push_back(*this);
                return;
            }

            auto shape = this->getShape();

            for (int i = 0; i < shape.num_dims(); i++)
            {
                if (shape[i] > 1)
                {
                    int64_t mid = shape[i] / 2;
                    
                    Region(startCoord, endCoord.with_i(i, startCoord[i] + mid  - 1)).split(result);
                    Region(startCoord.with_i(i, startCoord[i] + mid), endCoord).split(result);
                }
            }
        }

        Region align_with_symd_len(int64_t symd_len) const
        {
            auto last_dim_ind = endCoord.num_dims() - 1;

            auto width = endCoord[last_dim_ind] - startCoord[last_dim_ind] + 1;
            auto new_last_dim = endCoord[last_dim_ind] - (width % symd_len);

            return Region(startCoord, endCoord.with_i(last_dim_ind, new_last_dim));
        }
    };
}
