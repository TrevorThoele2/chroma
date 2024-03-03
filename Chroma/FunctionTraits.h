#pragma once

#include "Type.h"
#include "ParameterPack.h"
#include "ParameterIndex.h"

namespace Chroma
{
    template<class T>
    struct FunctionTraits;

    template<class Ret, class... Args>
    class Function;

    template<class Ret, class... Args>
    struct FunctionTraits<Ret(*)(Args...)>
    {
        using Type = Ret(*)(Args...);
        using FunctionT = Function<Ret, Args...>;

        using ReturnT = Ret;
        using ParameterPackT = ParameterPack<Args...>;
        static constexpr ParameterIndex parameterCount = ParameterPackT::count;

        template<ParameterIndex index>
        struct Parameter
        {
            using Type = typename DiscoverType<index, Args...>::Type;
        };

        static constexpr bool isFunction = true;
        static constexpr bool isFunctor = false;
        static constexpr bool isConstFunctor = false;
    };

    template<class Ret, class Obj, class... Args>
    struct FunctionTraits<Ret(Obj::*)(Args...)>
    {
        using Type = Ret(Obj::*)(Args...);
        using FunctionT = Function<Ret, Args...>;

        using ReturnT = Ret;
        using ObjectT = Obj;
        using ParameterPackT = ParameterPack<Args...>;
        static constexpr ParameterIndex parameterCount = ParameterPackT::count;

        template<ParameterIndex index>
        struct Parameter
        {
            using Type = typename DiscoverType<index, Args...>::Type;
        };

        static constexpr bool isFunction = false;
        static constexpr bool isFunctor = true;
        static constexpr bool isConstFunctor = false;
    };

    template<class Ret, class Obj, class... Args>
    struct FunctionTraits<Ret(Obj::*)(Args...) const>
    {
        using Type = Ret(Obj::*)(Args...) const;
        using FunctionT = Function<Ret, Args...>;

        using ReturnT = Ret;
        using ObjectT = Obj;
        using ParameterPackT = ParameterPack<Args...>;
        static constexpr ParameterIndex parameterCount = ParameterPackT::count;

        template<ParameterIndex index>
        struct Parameter
        {
            using Type = typename DiscoverType<index, Args...>::Type;
        };

        static constexpr bool isFunction = false;
        static constexpr bool isFunctor = false;
        static constexpr bool isConstFunctor = true;
    };
}

namespace std
{
    template<class F>
    class function;
}

namespace Chroma
{
    template<class Ret, class... Args>
    struct FunctionTraits<std::function<Ret(Args...)>>
    {
        using FunctionT = Function<Ret, Args...>;

        using ReturnT = Ret;
        using ParameterPackT = ParameterPack<Args...>;
        static constexpr ParameterIndex parameterCount = ParameterPackT::count;

        template<ParameterIndex index>
        struct Parameter
        {
            using Type = typename DiscoverType<index, Args...>::Type;
        };
    };
}