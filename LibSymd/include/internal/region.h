#pragma once
#include <vector>


namespace symd
{
namespace __internal__
{
    // Inclusive region
    struct Region
    {
        size_t startRow;
        size_t endRow;
        size_t startCol;
        size_t endCol;

        Region(int startR, int endR, int startC, int endC)
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
    };
}
}
