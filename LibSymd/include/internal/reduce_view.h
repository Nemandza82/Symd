#pragma once
#include "basic_views.h"
#include <mutex>
#include <functional>


namespace symd::views
{
    template <typename T, typename ReduceOperation>
    struct reduce_view
    {
        std::mutex _final_sum_mutex;
        std::function<void(const reduce_view<T, ReduceOperation>& self)> _finalizer;

        T _sum;
        __internal__::SymdRegister<T> _regSum;

    public:
        const size_t _width;
        const size_t _height;
        const T _startValue;

        const ReduceOperation _reduceOperation;

        reduce_view(size_t width, size_t height, const T& startValue, const ReduceOperation& reduceOperation)
            : _reduceOperation(reduceOperation)
            , _width(width)
            , _height(height)
            , _startValue(startValue)
        {
            _sum = startValue;
            _regSum = __internal__::SymdRegister<T>(startValue);

            _finalizer = [](const reduce_view<T, ReduceOperation>& self) {
            };
        }

        reduce_view(size_t width, size_t height, const T& startValue, const ReduceOperation& reduceOperation,
            std::function<void(const reduce_view& self)>&& finalizer)
            : _reduceOperation(reduceOperation)
            , _width(width)
            , _height(height)
            , _startValue(startValue)
        {
            _sum = startValue;
            _regSum = __internal__::SymdRegister<T>(startValue);
            _finalizer = std::move(finalizer);
        }

        void append(const T& x)
        {
            _sum = _reduceOperation(_sum, x);
        }

        void append(const __internal__::SymdRegister<T>& x)
        {
            _regSum = _reduceOperation(_regSum, x);
        }

        void threadSafeAppend(const T& x)
        {
            std::lock_guard<std::mutex> guard(_final_sum_mutex);
            _sum = _reduceOperation(_sum, x);
        }

        T getResult() const
        {
            auto res = _sum;

            for (int i = 0; i < __internal__::SYMD_LEN; i++)
            {
                res = _reduceOperation(res, _regSum[i]);
            }

            return res;
        }

        ~reduce_view()
        {
            _finalizer(*this);
        }
    };
}


namespace symd::__internal__
{
    template<typename T, typename ReduceOperation>
    auto sub_view(views::reduce_view<T, ReduceOperation>& reductor, const Region& region)
    {
        // sub_view from reduce_view is smaller reduce_view
        return views::reduce_view<T, ReduceOperation>(region.width(), region.height(), reductor._startValue, reductor._reduceOperation, 
            [&](const views::reduce_view<T, ReduceOperation>& self)
            {
                reductor.threadSafeAppend(self.getResult());
            });
    }


    template <typename T, typename ReduceOperation>
    size_t getWidth(const views::reduce_view<T, ReduceOperation>& reductor)
    {
        return reductor._width;
    }

    template <typename T, typename ReduceOperation>
    size_t getHeight(const views::reduce_view<T, ReduceOperation>& reductor)
    {
        return reductor._height;
    }

    template <typename T, typename ReduceOperation>
    size_t getPitch(const views::reduce_view<T, ReduceOperation>& reductor)
    {
        return reductor._width;
    }

    template <typename T, typename ReduceOperation>
    auto fetchData(const views::reduce_view<T, ReduceOperation>& reductor, size_t row, size_t col)
    {
        static_assert(false, "Cannot fetch from Reductor");
    }

    template <typename T, typename ReduceOperation>
    auto fetchVecData(const views::reduce_view<T, ReduceOperation>& reductor, size_t row, size_t col)
    {
        static_assert(false, "Cannot fetch from Reductor");
    }

    template <typename T, typename ReduceOperation>
    void saveData(views::reduce_view<T, ReduceOperation>& reductor, const T& element, size_t row, size_t col)
    {
        reductor.append(element);
    }

    template <typename T, typename ReduceOperation>
    void saveVecData(views::reduce_view<T, ReduceOperation>& reductor, const SymdRegister<T>& element, size_t row, size_t col)
    {
        reductor.append(element);
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Handle multiple outputs to reductor
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*template <typename T, typename ReduceOperation, int count>
    void saveVecData(views::reduce_view<T, ReduceOperation>& reductor, const std::array<__internal__::SymdRegister<T>, count>& element, size_t row, size_t col)
    {
        
    }*/
}
