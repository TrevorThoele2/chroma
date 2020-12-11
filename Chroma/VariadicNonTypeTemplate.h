#pragma once

#include "DiscoverNonType.h"
#include "VariadicTemplate.h"

namespace Chroma
{
    using VariadicTemplateSize = size_t;

    template<auto... Args>
    class VariadicNonTypeTemplate
    {
    public:
        static constexpr VariadicTemplateSize count = sizeof...(Args);

        template<VariadicTemplateSize index>
        struct Parameter
        {
            static constexpr auto nonType = DiscoverNonType<index, Args...>::nonType;
        };

        template<auto... PrependArgs>
        class Prepend
        {
        public:
            using Type = VariadicNonTypeTemplate<PrependArgs..., Args...>;
        };

        template<auto... AppendArgs>
        class Append
        {
        public:
            using Type = VariadicNonTypeTemplate<Args..., AppendArgs...>;
        };

        template<template<auto...> class ForwardTo>
        class ForwardArguments
        {
        public:
            constexpr static auto nonType = ForwardTo<Args...>::nonType;
        };

        // Transforms a parameter pack according to the trait passed
        // The transformer needs to have a static constexpr variable called nonType
        template<template<auto> class Transformer>
        class Transform
        {
        private:
            template<VariadicTemplateSize index, auto... HoldArgs>
            struct Impl
            {
                static constexpr auto pieceNonType = Transformer<Parameter<index - 1>::Type>::nonType;
                static constexpr auto nonType = Impl<index - 1, pieceNonType, HoldArgs...>::Type;
            };

            template<auto... HoldArgs>
            struct Impl<0, HoldArgs...>
            {
                using Type = VariadicNonTypeTemplate<HoldArgs...>;
            };
        public:
            using Type = typename Impl<count>::Type;
        };

        // Transforms a parameter pack according to the trait passed
        // The transformer needs to have a type called Type
        template<template<auto> class Transformer>
        class TransformToType
        {
        private:
            template<VariadicTemplateSize index, class... HoldArgs>
            struct Impl
            {
                using PieceType = typename Transformer<Parameter<index - 1>::nonType>::Type;
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

        class Reverse
        {
        private:
            template<VariadicTemplateSize index, auto... HoldArgs>
            struct TypeStep
            {
                static constexpr auto nonType = Parameter<count - index>::nonType;
                using Type = typename TypeStep<index - 1, nonType, HoldArgs...>::Type;
            };

            template<auto... HoldArgs>
            struct TypeStep<0, HoldArgs...>
            {
                using Type = VariadicNonTypeTemplate<HoldArgs...>;
            };
        public:
            using Type = typename TypeStep<count>::Type;
        };
    };
}