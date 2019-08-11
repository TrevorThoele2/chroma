#pragma once

#include <functional>

#include "ParameterPack.h"

namespace Chroma
{
    template<class Ret, class... Args>
    struct DiscoverStdFunction
    {
        using T = std::function<Ret(Args...)>;
    };

    template<class Ret, class... Args>
    struct DiscoverStdFunction<Ret, ParameterPack<Args...>>
    {
        using T = std::function<Ret(Args...)>;
    };
}