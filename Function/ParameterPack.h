#pragma once

#include "VariadicTemplate.h"

namespace function
{
    template<class... Args>
    using ParameterPack = VariadicTemplate<Args...>;
}