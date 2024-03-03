#pragma once

#include "Type.h"
#include "ParameterPack.h"
#include "ParameterIndex.h"

namespace function
{
    template<class T>
    struct FunctionTraits;

    template<class Ret, class... Args>
    class Function;

    template<class Ret, class... Args>
    struct FunctionTraits<Ret(*)(Args...)>
    {
        typedef Ret(*Type)(Args...);
        typedef Function<Ret, Args...> FunctionT;

        typedef Ret ReturnT;
        typedef ParameterPack<Args...> ParameterPackT;
        static constexpr ParameterIndex parameterCount = ParameterPackT::count;

        template<ParameterIndex index>
        struct Parameter
        {
            typedef typename DiscoverType<index, Args...>::Type Type;
        };

        static constexpr bool isFunction = true;
        static constexpr bool isFunctor = false;
        static constexpr bool isConstFunctor = false;
    };

    template<class Ret, class Obj, class... Args>
    struct FunctionTraits<Ret(Obj::*)(Args...)>
    {
        typedef Ret(Obj::*Type)(Args...);
        typedef Function<Ret, Args...> FunctionT;

        typedef Ret ReturnT;
        typedef Obj ObjectT;
        typedef ParameterPack<Args...> ParameterPackT;
        static constexpr ParameterIndex parameterCount = ParameterPackT::count;

        template<ParameterIndex index>
        struct Parameter
        {
            typedef typename DiscoverType<index, Args...>::Type Type;
        };

        static constexpr bool isFunction = false;
        static constexpr bool isFunctor = true;
        static constexpr bool isConstFunctor = false;
    };

    template<class Ret, class Obj, class... Args>
    struct FunctionTraits<Ret(Obj::*)(Args...) const>
    {
        typedef Ret(Obj::*Type)(Args...) const;
        typedef Function<Ret, Args...> FunctionT;

        typedef Ret ReturnT;
        typedef Obj ObjectT;
        typedef ParameterPack<Args...> ParameterPackT;
        static constexpr ParameterIndex parameterCount = ParameterPackT::count;

        template<ParameterIndex index>
        struct Parameter
        {
            typedef typename DiscoverType<index, Args...>::Type Type;
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

namespace function
{
    template<class Ret, class... Args>
    struct FunctionTraits<std::function<Ret(Args...)>>
    {
        typedef Function<Ret, Args...> FunctionT;

        typedef Ret ReturnT;
        typedef ParameterPack<Args...> ParameterPackT;
        static constexpr ParameterIndex parameterCount = ParameterPackT::count;

        template<ParameterIndex index>
        struct Parameter
        {
            typedef typename DiscoverType<index, Args...>::Type Type;
        };
    };
}