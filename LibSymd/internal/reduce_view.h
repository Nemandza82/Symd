#pragma once
#include "basic_views.h"
#include <mutex>
#include <functional>


namespace symd::views
{
    /// <summary>
    /// View used for reduction operation. Perform symd::map to this view in order to perform reductions.
    /// </summary>
    template <typename T, typename ReduceOperation>
    struct reduce_view
    {
        std::shared_ptr<std::mutex> _final_sum_mutex;
        std::function<void(const reduce_view<T, ReduceOperation>& self)> _finalizer;

        T _sum;
        __internal__::SymdRegister<T> _regSum;

    public:
        const size_t _width;
        const size_t _height;
        const T _startValue;

        const ReduceOperation _reduceOperation;

        /// <summary>
        /// Constructs empty reduce_view.
        /// </summary>
        reduce_view()
        {
        }

        /// <summary>
        /// Constructs reduce_view given reduce operation.
        /// </summary>
        /// <param name="width">Width of input view you want to perform reduction on.</param>
        /// <param name="height">Height of input view you want to perform reduction on.</param>
        /// <param name="startValue">Value for initializing operation / neutral element for operation. Eg 0 for addition or 1 for multiplication.</param>
        /// <param name="reduceOperation">Input operation lambda function.</param>
        reduce_view(size_t width, size_t height, const T& startValue, const ReduceOperation& reduceOperation)
            : _reduceOperation(reduceOperation)
            , _width(width)
            , _height(height)
            , _startValue(startValue)
        {
            _sum = startValue;
            _regSum = __internal__::SymdRegister<T>(startValue);

            _final_sum_mutex = std::make_shared<std::mutex>();
            _finalizer = [](const reduce_view<T, ReduceOperation>& self) {
            };
        }

        /// <summary>
        /// Constructs reduce_view given reduce operation and finalizer - DO NOT USE, FOR INTERNAL USE ONLY.
        /// </summary>
        /// <param name="width">Width of input view you want to perform reduction on.</param>
        /// <param name="height">Height of input view you want to perform reduction on.</param>
        /// <param name="startValue">Value for initializing operation / neutral element for operation. Eg 0 for addition or 1 for multiplication.</param>
        /// <param name="reduceOperation">Input operation lambda function.</param>
        /// <param name="finalizer">Lambda function to be executed on *this before this is destructed. Needed for correct parallel reduction implementation.</param>
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

        /// <summary>
        /// Appends input data to reduce_view. INTERNAL - DO NOT USE.
        /// </summary>
        /// <param name="x">Data to be appended.</param>
        void append(const T& x)
        {
            _sum = _reduceOperation(_sum, x);
        }

        /// <summary>
        /// Appends input SymdRegister to reduce_view. INTERNAL - DO NOT USE.
        /// </summary>
        /// <param name="x">SymdRegister to be appended.</param>
        void append(const __internal__::SymdRegister<T>& x)
        {
            _regSum = _reduceOperation(_regSum, x);
        }

        /// <summary>
        /// Appends input data to reduce_view, but is thread safe. INTERNAL - DO NOT USE.
        /// </summary>
        /// <param name="x">Data to be appended.</param>
        void threadSafeAppend(const T& x)
        {
            if (_final_sum_mutex)
            {
                std::lock_guard<std::mutex> guard(*_final_sum_mutex);
                _sum = _reduceOperation(_sum, x);
            }
        }

        /// <summary>
        /// Gets final result of reduction operation.
        /// </summary>
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

    /*template <typename T, typename ReduceOperation>
    auto fetchData(const views::reduce_view<T, ReduceOperation>& reductor, size_t row, size_t col)
    {
        static_assert(false, "Cannot fetch from Reductor");
    }

    template <typename T, typename ReduceOperation>
    auto fetchVecData(const views::reduce_view<T, ReduceOperation>& reductor, size_t row, size_t col)
    {
        static_assert(false, "Cannot fetch from Reductor");
    }*/

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

    /// <summary>
    /// Creates sub_view from underlying reduce_view.
    /// </summary>
    /// <param name="reductor">Underlying reduce_view.</param>
    /// <param name="region">Subview region.</param>
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
}
