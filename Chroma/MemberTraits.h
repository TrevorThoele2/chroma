#pragma once

namespace Chroma
{
    template<class T>
    struct MemberTraits;

    template<class Value, class Object>
    struct MemberTraits<Value Object::*>
    {
        using ValueT = Value;
        using ObjectT = Object;
    };
}