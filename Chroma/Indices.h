#pragma once

#include "ParameterIndex.h"

namespace Chroma
{
    template<class NumT, NumT... indices>
    struct Indices
    {
    private:
        template<NumT val, NumT... holder>
        struct Impl
        {
            static constexpr NumT Do()
            {
                return Impl<holder...>::Do();
            }
        };

        template<NumT val>
        struct Impl<val>
        {
            static constexpr NumT Do()
            {
                return val;
            }
        };
    public:
        template<ParameterIndex i>
        static constexpr NumT Get()
        {
            static_assert(i < sizeof...(indices), "Index is out of bounds.");
            return Impl<i>();
        }
    };

    template<ParameterIndex... indices>
    using ParameterIndices = Indices<ParameterIndex, indices...>;
}