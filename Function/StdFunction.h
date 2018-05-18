#pragma once

#include <functional>

#include "ParameterPack.h"

namespace function
{
    template<class Ret, class... Args>
    struct DiscoverStdFunction
    {
        typedef std::function<Ret(Args...)> T;
    };

    template<class Ret, class... Args>
    struct DiscoverStdFunction<Ret, ParameterPack<Args...>>
    {
        typedef std::function<Ret(Args...)> T;
    };
}