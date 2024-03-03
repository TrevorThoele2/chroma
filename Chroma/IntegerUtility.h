#pragma once

namespace Chroma
{
    template<class T>
    T Digits(T of)
    {
        const auto division = of / 10;
        return division == 0 ? 1 : 1 + Digits(division);
    }
}