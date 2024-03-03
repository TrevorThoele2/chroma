#pragma once

#include <tuple>
#include "ParameterIndex.h"

namespace Chroma
{
    template<class T>
    struct Type
    {};

    template<ParameterIndex index, class... Args>
    class DiscoverType
    {
    private:
        template<bool done, ParameterIndex id, class T, class... PassedArgs>
        struct Impl
        {
            typedef typename Impl<id + 1 == sizeof...(Args) || id + 1 == index, id + 1, PassedArgs...>::Type Type;
        };

        template<ParameterIndex id, class T, class... PassedArgs>
        struct Impl<true, id, T, PassedArgs...>
        {
            typedef T Type;
        };
    public:
        static_assert(index < sizeof...(Args), "Index is out of bounds.");
        typedef typename Impl<1 == sizeof...(Args) || 0 == index, 0, Args...>::Type Type;
    };
}