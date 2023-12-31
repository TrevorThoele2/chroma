#pragma once

#include "ParameterIndex.h"

namespace Chroma
{
    template<ParameterIndex index, class... Args>
    class DiscoverType
    {
    private:
        template<bool done, ParameterIndex id, class T, class... PassedArgs>
        struct Impl
        {
            using Type = typename Impl<id + 1 == sizeof...(Args) || id + 1 == index, id + 1, PassedArgs...>::Type;
        };

        template<ParameterIndex id, class T, class... PassedArgs>
        struct Impl<true, id, T, PassedArgs...>
        {
            using Type = T;
        };
    public:
        static_assert(index < sizeof...(Args), "Index is out of bounds.");
        using Type = typename Impl<1 == sizeof...(Args) || 0 == index, 0, Args...>::Type;
    };
}