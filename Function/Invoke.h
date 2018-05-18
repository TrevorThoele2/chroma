#pragma once

#include <functional>

#include "ParameterIndex.h"
#include "Function.h"
#include "FunctionTraits.h"

#include "RangeSize.h"

namespace function
{
    namespace detail
    {
        template<ParameterIndex index>
        struct InvokeImplementation
        {
            // GENERAL TUPLE
            template<class FunctionT, class... TupleArgs, class... HolderArgs>
            static void Do(FunctionT& function, std::tuple<TupleArgs...>& args, HolderArgs && ... holder)
            {
                InvokeImplementation<index - 1>::Do(function, args, std::forward<std::tuple_element<index - 1, std::remove_reference<decltype(args)>::type>::type>(std::get<index - 1>(args)), std::forward<HolderArgs>(holder)...);
            }

            // TUPLE, FUNCTOR
            template<class Ret, class Obj, class... Args, class... TupleArgs, class... HolderArgs>
            static void Do(Ret(Obj::*function)(Args...), Obj& obj, std::tuple<TupleArgs...>& args, HolderArgs && ... holder)
            {
                InvokeImplementation<index - 1>::Do(function, obj, args, std::forward<std::tuple_element<index - 1, std::remove_reference<decltype(args)>::type>::type>(std::get<index - 1>(args)), std::forward<HolderArgs>(holder)...);
            }

            // TUPLE, CONST FUNCTOR
            template<class Ret, class Obj, class... Args, class... TupleArgs, class... HolderArgs>
            static void Do(Ret(Obj::*function)(Args...) const, const Obj& obj, std::tuple<TupleArgs...>& args, HolderArgs && ... holder)
            {
                InvokeImplementation<index - 1>::Do(function, obj, args, std::forward<std::tuple_element<index - 1, std::remove_reference<decltype(args)>::type>::type>(std::get<index - 1>(args)), std::forward<HolderArgs>(holder)...);
            }

            // GENERAL ARRAY
            template<class FunctionT, RangeSize arraySize, class T, class... HolderArgs>
            static void Do(FunctionT& function, std::array<T, arraySize>& args, HolderArgs && ... holder)
            {
                InvokeImplementation<index - 1>::Do(function, args, std::forward<std::tuple_element<index - 1, std::remove_reference<decltype(args)>::type>::type>(std::get<index - 1>(args)), std::forward<HolderArgs>(holder)...);
            }

            // ARRAY, FUNCTOR
            template<class Ret, class Obj, class... Args, RangeSize arraySize, class T, class... HolderArgs>
            static void Do(Ret(Obj::*function)(Args...), Obj& obj, std::array<T, arraySize>& args, HolderArgs && ... holder)
            {
                InvokeImplementation<index - 1>::Do(function, obj, args, std::forward<std::tuple_element<index - 1, std::remove_reference<decltype(args)>::type>::type>(std::get<index - 1>(args)), std::forward<HolderArgs>(holder)...);
            }

            // ARRAY, CONST FUNCTOR
            template<class Ret, class Obj, class... Args, RangeSize arraySize, class T, class... HolderArgs>
            static void Do(Ret(Obj::*function)(Args...) const, const Obj& obj, std::array<T, arraySize>& args, HolderArgs && ... holder)
            {
                InvokeImplementation<index - 1>::Do(function, obj, args, std::forward<std::tuple_element<index - 1, std::remove_reference<decltype(args)>::type>::type>(std::get<index - 1>(args)), std::forward<HolderArgs>(holder)...);
            }
        };

        template<>
        struct InvokeImplementation<0>
        {
            // FREE FUNCTION, TUPLE
            template<class Ret, class... Args, class... TupleArgs, class... HolderArgs>
            static void Do(Ret(*function)(Args...), std::tuple<TupleArgs...>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
                (function)(std::forward<HolderArgs>(holder)...);
            }

            // FUNCTOR, TUPLE
            template<class Ret, class Obj, class... Args, class... TupleArgs, class... HolderArgs>
            static void Do(Ret(Obj::*function)(Args...), Obj& obj, std::tuple<TupleArgs...>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
                (obj.function)(std::forward<HolderArgs>(holder)...);
            }

            // CONST FUNCTOR, TUPLE
            template<class Ret, class Obj, class... Args, class... TupleArgs, class... HolderArgs>
            static void Do(Ret(Obj::*function)(Args...) const, const Obj& obj, std::tuple<TupleArgs...>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
                (obj.function)(std::forward<HolderArgs>(holder)...);
            }

            // FUNCTION, TUPLE
            template<class Ret, class... Args, class... TupleArgs, class... HolderArgs>
            static void Do(Function<Ret, Args...>& function, std::tuple<TupleArgs...>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
                function(std::forward<HolderArgs>(holder)...);
            }

            // STD::FUNCTION, TUPLE
            template<class Ret, class... Args, class... TupleArgs, class... HolderArgs>
            static void Do(std::function<Ret(Args...)>& function, std::tuple<TupleArgs...>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
                function(std::forward<HolderArgs>(holder)...);
            }

            // FREE FUNCTION, ARRAY
            template<class Ret, class... Args, RangeSize arraySize, class T, class... HolderArgs>
            static void Do(Ret(*function)(Args...), std::array<T, arraySize>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the tuple.");
                (function)(std::forward<HolderArgs>(holder)...);
            }

            // FUNCTOR, ARRAY
            template<class Ret, class Obj, class... Args, RangeSize arraySize, class T, class... HolderArgs>
            static void Do(Ret(Obj::*function)(Args...), Obj& obj, std::array<T, arraySize>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the tuple.");
                (obj.function)(std::forward<HolderArgs>(holder)...);
            }

            // CONST FUNCTOR, ARRAY
            template<class Ret, class Obj, class... Args, RangeSize arraySize, class T, class... HolderArgs>
            static void Do(Ret(Obj::*function)(Args...) const, const Obj& obj, std::array<T, arraySize>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the tuple.");
                (obj.function)(std::forward<HolderArgs>(holder)...);
            }

            // FUNCTION, ARRAY
            template<class Ret, class... Args, RangeSize arraySize, class T, class... HolderArgs>
            static void Do(Function<Ret, Args...>& function, std::array<T, arraySize>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the tuple.");
                function(std::forward<HolderArgs>(holder)...);
            }

            // STD::FUNCTION, ARRAY
            template<class Ret, class... Args, RangeSize arraySize, class T, class... HolderArgs>
            static void Do(std::function<Ret(Args...)>& function, std::array<T, arraySize>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the tuple.");
                function(std::forward<HolderArgs>(holder)...);
            }
        };

        // Returns
        template<ParameterIndex index>
        struct InvokeImplementationReturn
        {
            // GENERAL TUPLE
            template<class FunctionT, class... TupleArgs, class... HolderArgs>
            static typename FunctionTraits<FunctionT>::ReturnT Do(FunctionT& function, std::tuple<TupleArgs...>& args, HolderArgs && ... holder)
            {
                return InvokeImplementationReturn<index - 1>::Do(function, args, std::forward<std::tuple_element<index - 1, std::remove_reference<decltype(args)>::type>::type>(std::get<index - 1>(args)), std::forward<HolderArgs>(holder)...);
            }

            // TUPLE, FUNCTOR
            template<class Ret, class Obj, class... Args, class... TupleArgs, class... HolderArgs>
            static Ret Do(Ret(Obj::*function)(Args...), Obj& obj, std::tuple<TupleArgs...>& args, HolderArgs && ... holder)
            {
                return InvokeImplementationReturn<index - 1>::Do(function, obj, args, std::forward<std::tuple_element<index - 1, std::remove_reference<decltype(args)>::type>::type>(std::get<index - 1>(args)), std::forward<HolderArgs>(holder)...);
            }

            // TUPLE, CONST FUNCTOR
            template<class Ret, class Obj, class... Args, class... TupleArgs, class... HolderArgs>
            static Ret Do(Ret(Obj::*function)(Args...) const, const Obj& obj, std::tuple<TupleArgs...>& args, HolderArgs && ... holder)
            {
                return InvokeImplementationReturn<index - 1>::Do(function, obj, args, std::forward<std::tuple_element<index - 1, std::remove_reference<decltype(args)>::type>::type>(std::get<index - 1>(args)), std::forward<HolderArgs>(holder)...);
            }

            // GENERAL ARRAY
            template<class FunctionT, RangeSize arraySize, class T, class... HolderArgs>
            static typename FunctionTraits<FunctionT>::ReturnT Do(FunctionT& function, std::array<T, arraySize>& args, HolderArgs && ... holder)
            {
                return InvokeImplementationReturn<index - 1>::Do(function, args, std::forward<std::tuple_element<index - 1, std::remove_reference<decltype(args)>::type>::type>(std::get<index - 1>(args)), std::forward<HolderArgs>(holder)...);
            }

            // ARRAY, FUNCTOR
            template<class Ret, class Obj, class... Args, RangeSize arraySize, class T, class... HolderArgs>
            static Ret Do(Ret(Obj::*function)(Args...), Obj& obj, std::array<T, arraySize>& args, HolderArgs && ... holder)
            {
                return InvokeImplementationReturn<index - 1>::Do(function, obj, args, std::forward<std::tuple_element<index - 1, std::remove_reference<decltype(args)>::type>::type>(std::get<index - 1>(args)), std::forward<HolderArgs>(holder)...);
            }

            // ARRAY, CONST FUNCTOR
            template<class Ret, class Obj, class... Args, RangeSize arraySize, class T, class... HolderArgs>
            static Ret Do(Ret(Obj::*function)(Args...) const, const Obj& obj, std::array<T, arraySize>& args, HolderArgs && ... holder)
            {
                return InvokeImplementationReturn<index - 1>::Do(function, obj, args, std::forward<std::tuple_element<index - 1, std::remove_reference<decltype(args)>::type>::type>(std::get<index - 1>(args)), std::forward<HolderArgs>(holder)...);
            }
        };

        template<>
        struct InvokeImplementationReturn<0>
        {
            // FREE FUNCTION, TUPLE
            template<class Ret, class... Args, class... TupleArgs, class... HolderArgs>
            static Ret Do(Ret(*function)(Args...), std::tuple<TupleArgs...>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
                return (function)(std::forward<HolderArgs>(holder)...);
            }

            // FUNCTOR, TUPLE
            template<class Ret, class Obj, class... Args, class... TupleArgs, class... HolderArgs>
            static Ret Do(Ret(Obj::*function)(Args...), Obj& obj, std::tuple<TupleArgs...>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
                return (obj.function)(std::forward<HolderArgs>(holder)...);
            }

            // CONST FUNCTOR, TUPLE
            template<class Ret, class Obj, class... Args, class... TupleArgs, class... HolderArgs>
            static Ret Do(Ret(Obj::*function)(Args...) const, const Obj& obj, std::tuple<TupleArgs...>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
                return (obj.function)(std::forward<HolderArgs>(holder)...);
            }

            // FUNCTION, TUPLE
            template<class Ret, class... Args, class... TupleArgs, class... HolderArgs>
            static Ret Do(Function<Ret, Args...>& function, std::tuple<TupleArgs...>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
                return function(std::forward<HolderArgs>(holder)...);
            }

            // STD::FUNCTION, TUPLE
            template<class Ret, class... Args, class... TupleArgs, class... HolderArgs>
            static Ret Do(std::function<Ret(Args...)>& function, std::tuple<TupleArgs...>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
                return function(std::forward<HolderArgs>(holder)...);
            }

            // FREE FUNCTION, ARRAY
            template<class Ret, class... Args, RangeSize arraySize, class T, class... HolderArgs>
            static Ret Do(Ret(*function)(Args...), std::array<T, arraySize>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the tuple.");
                return (function)(std::forward<HolderArgs>(holder)...);
            }

            // FUNCTOR, ARRAY
            template<class Ret, class Obj, class... Args, RangeSize arraySize, class T, class... HolderArgs>
            static Ret Do(Ret(Obj::*function)(Args...), Obj& obj, std::array<T, arraySize>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the tuple.");
                return (obj.function)(std::forward<HolderArgs>(holder)...);
            }

            // CONST FUNCTOR, ARRAY
            template<class Ret, class Obj, class... Args, RangeSize arraySize, class T, class... HolderArgs>
            static Ret Do(Ret(Obj::*function)(Args...) const, const Obj& obj, std::array<T, arraySize>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the tuple.");
                return (obj.function)(std::forward<HolderArgs>(holder)...);
            }

            // FUNCTION, ARRAY
            template<class Ret, class... Args, RangeSize arraySize, class T, class... HolderArgs>
            static Ret Do(Function<Ret, Args...>& function, std::array<T, arraySize>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the tuple.");
                return function(std::forward<HolderArgs>(holder)...);
            }

            // STD::FUNCTION, ARRAY
            template<class Ret, class... Args, RangeSize arraySize, class T, class... HolderArgs>
            static Ret Do(std::function<Ret(Args...)>& function, std::array<T, arraySize>& args, HolderArgs && ... holder)
            {
                static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the tuple.");
                return function(std::forward<HolderArgs>(holder)...);
            }
        };
    }

    template<class Ret, class... Args, class... TupleArgs>
    void Invoke(Ret(*function)(Args...), std::tuple<TupleArgs...>& args)
    {
        static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
        detail::InvokeImplementation<sizeof...(Args)>::Do(function, args);
    }

    template<class Ret, class Obj, class... Args, class... TupleArgs>
    void Invoke(Ret(Obj::*function)(Args...), Obj& obj, std::tuple<TupleArgs...>& args)
    {
        static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
        detail::InvokeImplementation<sizeof...(Args)>::Do(function, obj, args);
    }

    template<class Ret, class Obj, class... Args, class... TupleArgs>
    void Invoke(Ret(Obj::*function)(Args...) const, const Obj& obj, std::tuple<TupleArgs...>& args)
    {
        static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
        detail::InvokeImplementation<sizeof...(Args)>::Do(function, obj, args);
    }

    template<class Ret, class... Args, class... TupleArgs>
    void Invoke(Function<Ret, Args...>& function, std::tuple<TupleArgs...>& args)
    {
        static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
        detail::InvokeImplementation<sizeof...(Args)>::Do(function, args);
    }

    template<class Ret, class... Args, class... TupleArgs>
    void Invoke(std::function<Ret(Args...)>& function, std::tuple<TupleArgs...>& args)
    {
        static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
        detail::InvokeImplementation<sizeof...(Args)>::Do(function, args);
    }

    template<class Ret, class... Args, RangeSize arraySize, class T>
    void Invoke(Ret(*function)(Args...), std::array<T, arraySize>& args)
    {
        static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the array.");
        detail::InvokeImplementation<sizeof...(Args)>::Do(function, args);
    }

    template<class Ret, class Obj, class... Args, RangeSize arraySize, class T>
    void Invoke(Ret(Obj::*function)(Args...), Obj& obj, std::array<T, arraySize>& args)
    {
        static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the array.");
        detail::InvokeImplementation<sizeof...(Args)>::Do(function, obj, args);
    }

    template<class Ret, class Obj, class... Args, RangeSize arraySize, class T>
    void Invoke(Ret(Obj::*function)(Args...), const Obj& obj, std::array<T, arraySize>& args)
    {
        static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the array.");
        detail::InvokeImplementation<sizeof...(Args)>::Do(function, obj, args);
    }

    template<class Ret, class... Args, RangeSize arraySize, class T>
    void Invoke(Function<Ret, Args...>& function, std::array<T, arraySize>& args)
    {
        static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the array.");
        detail::InvokeImplementation<sizeof...(Args)>::Do(function, args);
    }

    template<class Ret, class... Args, RangeSize arraySize, class T>
    void Invoke(std::function<Ret(Args...)>& function, std::array<T, arraySize>& args)
    {
        static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the array.");
        detail::InvokeImplementation<sizeof...(Args)>::Do(function, args);
    }

    template<class Ret, class... Args, class... TupleArgs>
    Ret InvokeReturn(Ret(*function)(Args...), std::tuple<TupleArgs...>& args)
    {
        static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
        return detail::InvokeImplementationReturn<sizeof...(Args)>::Do(function, args);
    }

    template<class Ret, class Obj, class... Args, class... TupleArgs>
    Ret InvokeReturn(Ret(Obj::*function)(Args...), Obj& obj, std::tuple<TupleArgs...>& args)
    {
        static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
        return detail::InvokeImplementationReturn<sizeof...(Args)>::Do(function, obj, args);
    }

    template<class Ret, class Obj, class... Args, class... TupleArgs>
    Ret InvokeReturn(Ret(Obj::*function)(Args...) const, const Obj& obj, std::tuple<TupleArgs...>& args)
    {
        static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
        return detail::InvokeImplementationReturn<sizeof...(Args)>::Do(function, obj, args);
    }

    template<class Ret, class... Args, class... TupleArgs>
    Ret InvokeReturn(Function<Ret, Args...>& function, std::tuple<TupleArgs...>& args)
    {
        static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
        return detail::InvokeImplementationReturn<sizeof...(Args)>::Do(function, args);
    }

    template<class Ret, class... Args, class... TupleArgs>
    Ret InvokeReturn(std::function<Ret(Args...)>& function, std::tuple<TupleArgs...>& args)
    {
        static_assert(sizeof...(Args) == sizeof...(TupleArgs), "The argument pack must be exactly the same size as the size of the tuple.");
        return detail::InvokeImplementationReturn<sizeof...(Args)>::Do(function, args);
    }

    template<class Ret, class... Args, RangeSize arraySize, class T>
    Ret InvokeReturn(Ret(*function)(Args...), std::array<T, arraySize>& args)
    {
        static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the array.");
        return detail::InvokeImplementationReturn<sizeof...(Args)>::Do(function, args);
    }

    template<class Ret, class Obj, class... Args, RangeSize arraySize, class T>
    Ret InvokeReturn(Ret(Obj::*function)(Args...), Obj& obj, std::array<T, arraySize>& args)
    {
        static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the array.");
        return detail::InvokeImplementationReturn<sizeof...(Args)>::Do(function, obj, args);
    }

    template<class Ret, class Obj, class... Args, RangeSize arraySize, class T>
    Ret InvokeReturn(Ret(Obj::*function)(Args...), const Obj& obj, std::array<T, arraySize>& args)
    {
        static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the array.");
        return detail::InvokeImplementationReturn<sizeof...(Args)>::Do(function, obj, args);
    }

    template<class Ret, class... Args, RangeSize arraySize, class T>
    Ret InvokeReturn(Function<Ret, Args...>& function, std::array<T, arraySize>& args)
    {
        static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the array.");
        return detail::InvokeImplementationReturn<sizeof...(Args)>::Do(function, args);
    }

    template<class Ret, class... Args, RangeSize arraySize, class T>
    Ret InvokeReturn(std::function<Ret(Args...)>& function, std::array<T, arraySize>& args)
    {
        static_assert(sizeof...(Args) == arraySize, "The argument pack must be exactly the same size as the size of the array.");
        return detail::InvokeImplementationReturn<sizeof...(Args)>::Do(function, args);
    }
}