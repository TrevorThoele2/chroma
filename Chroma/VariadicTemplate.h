#pragma once

#include <tuple>
#include <array>

#include "Type.h"

namespace Chroma
{
    typedef size_t VariadicTemplateSize;

    template<class... Args>
    class VariadicTemplate
    {
    public:
        static constexpr VariadicTemplateSize count = sizeof...(Args);

        template<VariadicTemplateSize index>
        struct Parameter
        {
            typedef typename DiscoverType<index, Args...>::Type Type;
        };

        typedef std::tuple<Args...> TupleT;
        template<class PieceT>
        using ArrayT = std::array<PieceT, count>;

        template<class... PassArgs>
        static constexpr TupleT CreateTuple(PassArgs && ... pass) noexcept;
        template<class ParameterT, class... PassArgs>
        static constexpr ArrayT<ParameterT> CreateArray(PassArgs && ... pass) noexcept;

        // Transforms a parameter pack according to the trait passed
        // The transformer needs to have a type called Type
        template<template<class> class Transformer>
        class Transform
        {
        private:
            template<VariadicTemplateSize index, class... HoldArgs>
            struct Impl
            {
                typedef typename Transformer<typename Parameter<index - 1>::Type>::Type PieceType;
                typedef typename Impl<index - 1, PieceType, HoldArgs...>::Type Type;
            };

            template<class... HoldArgs>
            struct Impl<0, HoldArgs...>
            {
                typedef VariadicTemplate<HoldArgs...> Type;
            };
        public:
            typedef typename Impl<count>::Type Type;
        };

        template<class... PrependArgs>
        class Prepend
        {
        public:
            typedef VariadicTemplate<PrependArgs..., Args...> Type;
        };

        template<class... AppendArgs>
        class Append
        {
        public:
            typedef VariadicTemplate<Args..., AppendArgs...> Type;
        };

        template<template<class...> class ForwardTo>
        class ForwardArguments
        {
        public:
            typedef ForwardTo<Args...> Type;
        };

        template<class T>
        class IsTypeInside
        {
        private:
            template<VariadicTemplateSize index>
            struct TypeStep
            {
                typedef typename Parameter<index - 1>::Type PieceType;
                static constexpr bool areTypesSame = std::is_same<T, PieceType>::value;
                // Check if types are the same - if they are, just return true, otherwise check lower down
                static constexpr bool result = (areTypesSame) ? true : TypeStep<index - 1>::result;
            };

            template<>
            struct TypeStep<0>
            {
                typedef typename Parameter<0>::Type PieceType;
                static constexpr bool areTypesSame = std::is_same<T, PieceType>::value;
                static constexpr bool result = (areTypesSame) ? true : false;
            };
        public:
            static constexpr bool value = TypeStep<count>::result;
        };

        template<class T>
        class IsConvertibleTypeInside
        {
        private:
            template<VariadicTemplateSize index>
            struct TypeStep
            {
                typedef typename Parameter<index - 1>::Type PieceType;
                static constexpr bool areTypesConvertible = std::is_convertible<T, PieceType>::value;
                // Check if types are the same - if they are, just return true, otherwise check lower down
                static constexpr bool result = (areTypesConvertible) ? true : TypeStep<index - 1>::result;
            };

            template<>
            struct TypeStep<0>
            {
                typedef typename Parameter<0>::Type PieceType;
                static constexpr bool areTypesConvertible = std::is_convertible<T, PieceType>::value;
                static constexpr bool result = (areTypesConvertible) ? true : false;
            };
        public:
            static constexpr bool value = TypeStep<count>::result;
        };
    };

    template<class... Args>
    template<class... PassArgs>
    typename constexpr VariadicTemplate<Args...>::TupleT VariadicTemplate<Args...>::CreateTuple(PassArgs && ... pass) noexcept
    {
        return TupleT(std::forward<PassArgs>(pass)...);
    }

    template<class... Args>
    template<class ParameterT, class... PassArgs>
    typename constexpr VariadicTemplate<Args...>::ArrayT<ParameterT> VariadicTemplate<Args...>::CreateArray(PassArgs && ... pass) noexcept
    {
        return ArrayT<ParameterT>({ pass... });
    }

    template<class... Args>
    class ToVariadicTemplate;

    template<class... Args>
    class ToVariadicTemplate<std::tuple<Args...>>
    {
    public:
        typedef VariadicTemplate<Args...> Type;
    };
}