#pragma once

#include <tuple>
#include <array>

#include "DiscoverType.h"

namespace Chroma
{
    using VariadicTemplateSize = size_t;

    struct NonExistent
    {};

    template<class... Args>
    class VariadicTemplate
    {
    public:
        static constexpr VariadicTemplateSize count = sizeof...(Args);

        template<VariadicTemplateSize index>
        struct Parameter
        {
            using Type = typename DiscoverType<index, Args...>::Type;
        };

        using TupleT = std::tuple<Args...>;
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
                using PieceType = typename Transformer<typename Parameter<index - 1>::Type>::Type;
                using Type = typename Impl<index - 1, PieceType, HoldArgs...>::Type;
            };

            template<class... HoldArgs>
            struct Impl<0, HoldArgs...>
            {
                using Type = VariadicTemplate<HoldArgs...>;
            };
        public:
            using Type = typename Impl<count>::Type;
        };

        template<class... PrependArgs>
        class Prepend
        {
        public:
            using Type = VariadicTemplate<PrependArgs..., Args...>;
        };

        template<class... AppendArgs>
        class Append
        {
        public:
            using Type = VariadicTemplate<Args..., AppendArgs...>;
        };

        template<template<class...> class ForwardTo>
        class ForwardArguments
        {
        public:
            using Type = ForwardTo<Args...>;
        };

        template<class T>
        class IsTypeInside
        {
        private:
            template<VariadicTemplateSize index>
            struct Step
            {
                using PieceType = typename Parameter<index - 1>::Type;
                static constexpr bool areTypesSame = std::is_same<T, PieceType>::value;
                static constexpr bool result = areTypesSame ? true : Step<index - 1>::result;
            };

            template<>
            struct Step<0>
            {
                using PieceType = typename Parameter<0>::Type;
                static constexpr bool areTypesSame = std::is_same<T, PieceType>::value;
                static constexpr bool result = areTypesSame ? true : false;
            };
        public:
            static constexpr bool value = Step<count>::result;
        };

        template<class T>
        class TypeCount
        {
        private:
            template<VariadicTemplateSize index>
            struct Step
            {
                using PieceType = typename Parameter<index - 1>::Type;
                static constexpr bool areTypesSame = std::is_same<T, PieceType>::value;
                static constexpr VariadicTemplateSize result = areTypesSame ? Step<index - 1>::result + 1 : Step<index - 1>::result;
            };

            template<>
            struct Step<0>
            {
                static constexpr VariadicTemplateSize result = 0;
            };
        public:
            static constexpr VariadicTemplateSize value = Step<count>::result;
        };

        class AllUnique
        {
        private:
            template<VariadicTemplateSize index>
            struct Step
            {
                using PieceType = typename Parameter<index - 1>::Type;
                static constexpr VariadicTemplateSize count = TypeCount<PieceType>::value;
                static constexpr VariadicTemplateSize result = count > 1 ? false : Step<index - 1>::result;
            };

            template<>
            struct Step<0>
            {
                static constexpr VariadicTemplateSize result = true;
            };
        public:
            static constexpr bool value = Step<count>::result;
        };

        template<class T>
        class IsConvertibleTypeInside
        {
        private:
            template<VariadicTemplateSize index>
            struct TypeStep
            {
                using PieceType = typename Parameter<index - 1>::Type;
                static constexpr bool areTypesConvertible = std::is_convertible<T, PieceType>::value;
                // Check if types are the same - if they are, just return true, otherwise check lower down
                static constexpr bool result = (areTypesConvertible) ? true : TypeStep<index - 1>::result;
            };

            template<>
            struct TypeStep<0>
            {
                using PieceType = typename Parameter<0>::Type;
                static constexpr bool areTypesConvertible = std::is_convertible<T, PieceType>::value;
                static constexpr bool result = (areTypesConvertible) ? true : false;
            };
        public:
            static constexpr bool value = TypeStep<count>::result;
        };

        template<template<class> class Predicate>
        class Filter
        {
        private:
            template<VariadicTemplateSize index, class... DecidedTypes>
            struct TypeStep
            {
                using PieceType = typename Parameter<index - 1>::Type;
                static constexpr bool passes = Predicate<PieceType>::value;
                using Types = std::conditional_t<
                    passes,
                    typename TypeStep<index - 1, DecidedTypes..., PieceType>::Types,
                    typename TypeStep<index - 1, DecidedTypes...>::Types>;
            };

            template<class... DecidedTypes>
            struct TypeStep<0, DecidedTypes...>
            {
                using Types = VariadicTemplate<DecidedTypes...>;
            };
        public:
            using Types = typename TypeStep<count>::Types;
        };

        template<template<class> class Predicate>
        using filter = typename Filter<Predicate>::Types;

        template<class T, VariadicTemplateSize Index>
        struct FilterWithIndicesResult
        {
            using Type = T;
            static constexpr bool index = Index;
        };

        template<template<class> class Predicate>
        class FilterWithIndices
        {
        private:
            template<VariadicTemplateSize index, class... DecidedTypes>
            struct TypeStep
            {
                using PieceType = typename Parameter<index - 1>::Type;
                static constexpr bool passes = Predicate<PieceType>::value;
                using Types = std::conditional_t<
                    passes,
                    typename TypeStep<index - 1, DecidedTypes..., FilterWithIndicesResult<PieceType, index - 1>>::Types,
                    typename TypeStep<index - 1, DecidedTypes...>::Types>;
            };

            template<class... DecidedTypes>
            struct TypeStep<0, DecidedTypes...>
            {
                using Types = VariadicTemplate<DecidedTypes...>;
            };
        public:
            using Types = typename TypeStep<count>::Types;
        };

        template<template<class> class Predicate>
        using filter_with_indices = typename FilterWithIndices<Predicate>::Types;
    };

    template<class... Args>
    template<class... PassArgs>
    constexpr auto VariadicTemplate<Args...>::CreateTuple(PassArgs && ... pass) noexcept
        -> TupleT
    {
        return TupleT(std::forward<PassArgs>(pass)...);
    }

    template<class... Args>
    template<class ParameterT, class... PassArgs>
    constexpr auto VariadicTemplate<Args...>::CreateArray(PassArgs && ... pass) noexcept
        -> ArrayT<ParameterT>
    {
        return ArrayT<ParameterT>({ pass... });
    }

    template<class... Args>
    class ToVariadicTemplate;

    template<class... Args>
    class ToVariadicTemplate<std::tuple<Args...>>
    {
    public:
        using Type = VariadicTemplate<Args...>;
    };
}