#pragma once
#include "symd_register.h"
//#include "basic_views.h"
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


    template<typename Tuple, typename Elements, size_t... I>
    void saveDataImpl(Tuple& res, const Elements& element, size_t row, size_t col, std::index_sequence<I...>)
    {
        (saveData(std::get<I>(res), element[I], row, col), ...);
    }

    template <typename R, typename... Results>
    void saveData(std::tuple<Results...>& res, const std::array<R, sizeof...(Results)>& element, size_t row, size_t col)
    {
        saveDataImpl(res, element, row, col, std::make_index_sequence<sizeof...(Results)>{});
    }

    template<typename Tuple, typename Elements, size_t... I>
    void saveVecDataImpl(Tuple& res, const Elements& element, size_t row, size_t col, std::index_sequence<I...>)
    {
        (saveVecData(std::get<I>(res), element[I], row, col), ...);
    }

    template <typename R, typename... Results>
    void saveVecData(std::tuple<Results...>& res, const std::array<SymdRegister<R>, sizeof...(Results)>& element, size_t row, size_t col)
    {
        saveVecDataImpl(res, element, row, col, std::make_index_sequence<sizeof...(Results)>{});
    }
}
