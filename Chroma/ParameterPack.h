#pragma once

#include "VariadicTemplate.h"

namespace Chroma
{
    template<class... Args>
    using ParameterPack = VariadicTemplate<Args...>;
}