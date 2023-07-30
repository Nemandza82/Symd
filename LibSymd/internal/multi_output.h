#pragma once
#include "symd_register.h"
#include "region.h"
#include <utility>
#include <array>


namespace symd::__internal__
{
    template <typename... Views>
    Dimensions getShape(const std::tuple<Views...>& views)
    {
        return getShape(std::get<0>(views));
    }

    template <typename View, size_t N>
    Dimensions getShape(const std::array<View, N>& views, typename std::enable_if<!std::is_fundamental_v<View>, View>::type* = 0)
    {
        return getShape(views[0]);
    }

    template<typename Tuple, typename Elements, size_t... I>
    void saveDataImpl(Tuple& views, const Elements& elements, const Dimensions& coords, std::index_sequence<I...>)
    {
        (saveData(std::get<I>(views), elements[I], coords), ...);
    }

    template <typename R, typename... Views>
    void saveData(std::tuple<Views...>& views, const std::array<R, sizeof...(Views)>& element, const Dimensions& coords)
    {
        saveDataImpl(views, element, coords, std::make_index_sequence<sizeof...(Views)>{});
    }

    template <typename R, typename View, size_t N>
    void saveData(std::array<View, N>& views, const std::array<R, N>& elements, const Dimensions& coords)
    {
        for (size_t i = 0; i < N; i++)
            saveData(views[i], elements[i], coords);
    }

    template<typename Tuple, typename Elements, size_t... I>
    void saveVecDataImpl(Tuple& views, const Elements& elements, const Dimensions& coords, std::index_sequence<I...>)
    {
        (saveVecData(std::get<I>(views), elements[I], coords), ...);
    }

    template <typename R, typename... Views>
    void saveVecData(std::tuple<Views...>& views, const std::array<SymdRegister<R>, sizeof...(Views)>& elements, const Dimensions& coords)
    {
        saveVecDataImpl(views, elements, coords, std::make_index_sequence<sizeof...(Views)>{});
    }

    template <typename R, typename View, size_t N>
    void saveVecData(std::array<View, N>& views, const std::array<SymdRegister<R>, N>& elements, const Dimensions& coords)
    {
        for (size_t i = 0; i < N; i++)
            saveVecData(views[i], elements[i], coords);
    }

    template <typename View, size_t N, size_t... I>
    auto sub_viewImpl(std::array<View, N>& views, const Region& region, std::index_sequence<I...>)
    {
        using SubViewT = std::decay_t<decltype(sub_view(views[0], region))>;

        return std::array<SubViewT, N>
        {
            {sub_view(views[I], region) ...}
        };
    }

    template <typename View, size_t N>
    auto sub_view(std::array<View, N>& views, const Region& region)
    {
        return sub_viewImpl(views, region, std::make_index_sequence<N>{});
    }

    
    template <typename Tuple, size_t... I>
    auto sub_view_tuple_Impl(Tuple& views, const Region& region, std::index_sequence<I...>)
    {
        return std::make_tuple
        (
            sub_view(std::get<I>(views), region) ...
        );
    }

    template <typename... Views>
    auto sub_view(std::tuple<Views...>& views, const Region& region)
    {
        return sub_view_tuple_Impl(views, region, std::make_index_sequence<sizeof...(Views)>{});
    }
}
