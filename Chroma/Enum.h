#pragma once

#include <type_traits>
#include <tuple>

namespace Chroma
{
    namespace detail
    {
        template<class EnumType, EnumType max, EnumType min, template<EnumType> class T>
        class TupleTypeExpander
        {
        public:
            using Underlying = typename std::underlying_type<EnumType>::type;
            static constexpr Underlying maxU = static_cast<Underlying>(max);
            static constexpr Underlying minU = static_cast<Underlying>(min);
        private:
            static_assert(maxU > minU, "Max must be greater than min.");

            template<unsigned int index, class... HoldArgs>
            struct Impl
            {
                using PieceType = T<static_cast<EnumType>(index - 1)>;
                using MadeT = typename Impl<index - 1, PieceType, HoldArgs...>::MadeT;
            };

            template<class... HoldArgs>
            struct Impl<0, HoldArgs...>
            {
                using MadeT = std::tuple<HoldArgs...>;
            };
        public:
            using Type = typename Impl<maxU>::MadeT;
        };
    }

    template<class EnumType, EnumType max, EnumType min, template<EnumType> class T>
    using EnumToTuple = typename detail::TupleTypeExpander<EnumType, max, min, T>::Type;

    template<class EnumType, EnumType beginPass, EnumType endPass>
    struct EnumIterationTraits
    {
        using EnumT = EnumType;
        using UnderlyingType = typename std::underlying_type<EnumType>::type;

        static constexpr EnumType begin = beginPass;
        static constexpr EnumType end = endPass;

        static constexpr UnderlyingType beginU = static_cast<UnderlyingType>(begin);
        static constexpr UnderlyingType endU = static_cast<UnderlyingType>(end);

        static constexpr EnumType max = (beginPass > endPass) ? beginPass : endPass;
        static constexpr EnumType min = (beginPass < endPass) ? beginPass : endPass;

        static constexpr UnderlyingType maxU = static_cast<UnderlyingType>(max);
        static constexpr UnderlyingType minU = static_cast<UnderlyingType>(min);

        template<template<EnumType> class T>
        using ToTuple = typename detail::TupleTypeExpander<EnumType, max, min, T>::Type;
    };
}