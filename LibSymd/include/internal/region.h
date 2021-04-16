#pragma once
#include <vector>
#include <utility>


namespace symd
{
namespace __internal__
{
    /// <summary>
    /// Inclusive region. Used to help parallel compute.
    /// </summary>
    struct Region
    {
        size_t startRow;
        size_t endRow;
        size_t startCol;
        size_t endCol;

        Region(size_t startR, size_t endR, size_t startC, size_t endC)
        {
            startRow = startR;
            endRow = endR;
            startCol = startC;
            endCol = endC;
        }

        Region(size_t width, size_t height)
        {
            startRow = 0;
            endRow = height - 1;
            startCol = 0;
            endCol = width - 1;
        }

        /// <summary>
        /// Width of Region
        /// </summary>
        size_t width() const
        {
            return endCol - startCol + 1;
        }

        /// <summary>
        /// Height of Region
        /// </summary>
        size_t height() const
        {
            return endRow - startRow + 1;
        }

        /// <summary>
        /// Number of elements in Region
        /// </summary>
        size_t count() const
        {
            return width() * height();
        }

        /// <summary>
        /// Splits the Region in two roughly equal parts.
        /// </summary>
        /// <returns>Two disjoint regions with cover the source region.</returns>
        void split(std::vector<Region>& result) const
        {
            if (count() < 10000)
            {
                result.push_back(*this);
            }
            else if (height() > 1)
            {
                size_t mid = height() / 2;

                Region(startRow, startRow + mid - 1, startCol, endCol).split(result);
                Region(startRow + mid, endRow, startCol, endCol).split(result);
            }
            else // height() == 1
            {
                size_t mid = width() / 2;

                Region(startRow, endRow, startCol, startCol + mid - 1).split(result);
                Region(startRow, endRow, startCol + mid, endCol).split(result);
            }
        }
    };
}
}
