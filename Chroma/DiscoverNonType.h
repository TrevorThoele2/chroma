#pragma once

#include "ParameterIndex.h"

namespace Chroma
{
    template<ParameterIndex index, auto... Args>
    class DiscoverNonType
    {
    private:
        template<bool done, ParameterIndex id, auto Arg, auto... PassedArgs>
        struct Impl
        {
            static constexpr auto nonType = Impl<id + 1 == sizeof...(Args) || id + 1 == index, id + 1, PassedArgs...>::nonType;
        };

        template<ParameterIndex id, auto Arg, auto... PassedArgs>
        struct Impl<true, id, Arg, PassedArgs...>
        {
            static constexpr auto nonType = Arg;
        };
    public:
        static_assert(index < sizeof...(Args), "Index is out of bounds.");
        static constexpr auto nonType = Impl<1 == sizeof...(Args) || 0 == index, 0, Args...>::nonType;
    };
}