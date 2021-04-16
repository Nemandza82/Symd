#pragma once
#include "symd_register.h"
#include "../views.h"
#include <utility>


namespace symd::__internal__
{
    template <typename... Results>
    size_t getWidth(const std::tuple<Results...>& res)
    {
        return getWidth(std::get<0>(res));
    }

    template <typename... Results>
    size_t getHeight(const std::tuple<Results...>& res)
    {
        return getHeight(std::get<0>(res));
    }

    /*
    template <typename T1, typename T2, size_t... I>
    void transformTuple(T1&& s, T2& t, const Region& region, std::index_sequence<I...>)
    {
        (void)std::initializer_list<int>
        {
            (std::get<indices>(t) = subView(std::get<I>(s), region))...
        };
    }

    template<typename... Results>
    auto subView(std::tuple<Results...>& view, const Region& region)
    {
        transformTuple(view, data, region, std::make_index_sequence<sizeof...(Results)>{});
    }

    template<typename... Results, typename R>
    auto subView(std::tuple<Results...>& view, const Region& region)
    {
        std::tuple<data_view<R, 2>>
        return data_view<std::decay_t<decltype(*dataPtr)>, 2>(dataPtr, region.width(), region.height(), pitch);
    }
    */


    template<typename Tuple, typename A, size_t... I>
    void saveDataImpl(Tuple& res, const A& arr, size_t row, size_t col, std::index_sequence<I...>)
    {
        (saveData(std::get<I>(res), arr[I], row, col), ...);
    }

    template <typename R, typename... Results>
    void saveData(std::tuple<Results...>& res, const std::array<R, sizeof...(Results)>& data, size_t row, size_t col)
    {
        saveDataImpl(res, data, row, col, std::make_index_sequence<sizeof...(Results)>{});
    }

    template<typename Tuple, typename A, size_t... I>
    void saveVecDataImpl(Tuple& res, const A& arr, size_t row, size_t col, std::index_sequence<I...>)
    {
        (saveVecData(std::get<I>(res), arr[I], row, col), ...);
    }

    template <typename R, typename... Results>
    void saveVecData(std::tuple<Results...>& res, std::array<SymdRegister<R>, sizeof...(Results)>& data, size_t row, size_t col)
    {
        saveVecDataImpl(res, data, row, col, std::make_index_sequence<sizeof...(Results)>{});
    }
}
