#pragma once
#include <cassert>
#include "symd_register.h"
#include "../dimensions.h"
#include "std_vector_view.h"
#include "std_array_view.h"
#include "data_view.h"
#include "reduce_view.h"


namespace symd::__internal__
{
    template <typename View>
    auto fetchData(const View& view, const Dimensions& coords)
    {
        auto ptr = getDataPtr(view, coords);
        return *ptr;
    }

    template <typename View>
    auto fetchVecData(const View& view, const Dimensions& coords)
    {
        auto* ptr = getDataPtr(view, coords);
        return SymdRegister<std::decay_t<decltype(*ptr)>>(ptr);
    }


    template <typename View, typename DataType>
    void saveData(View& outView, const DataType& element, const Dimensions& coords)
    {
        auto* ptr = getDataPtr(outView, coords);
        *ptr = element;
    }

    template <typename View, typename DataType>
    void saveVecData(View& outView, const SymdRegister<DataType>& element, const Dimensions& coords)
    {
        auto* ptr = getDataPtr(outView, coords);
        element.store(ptr);
    }

    template <typename View>
    Dimensions getBorder(const View& input)
    {
        return getShape(input).native_border();
    }
}
