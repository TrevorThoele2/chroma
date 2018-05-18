#pragma once

#include <type_traits>
#include <tuple>

namespace function
{
    namespace detail
    {
        template<class EnumType, EnumType max, EnumType min, template<EnumType> class T>
        class TupleTypeExpander
        {
        public:
            typedef typename std::underlying_type<EnumType>::type Underlying;
            static constexpr Underlying maxU = static_cast<Underlying>(max);
            static constexpr Underlying minU = static_cast<Underlying>(min);
        private:
            static_assert(maxU > minU, "Max must be greater than min.");

            template<unsigned int index, class... HoldArgs>
            struct Impl
            {
                typedef T<static_cast<EnumType>(index - 1)> PieceType;
                typedef typename Impl<index - 1, PieceType, HoldArgs...>::MadeT MadeT;
            };

            template<class... HoldArgs>
            struct Impl<0, HoldArgs...>
            {
                typedef std::tuple<HoldArgs...> MadeT;
            };
        public:
            typedef typename Impl<maxU>::MadeT Type;
        };
    }

    template<class EnumType, EnumType max, EnumType min, template<EnumType> class T>
    using EnumToTuple = typename detail::TupleTypeExpander<EnumType, max, min, T>::Type;

    template<class EnumType, EnumType beginPass, EnumType endPass>
    struct EnumIterationTraits
    {
        typedef EnumType EnumT;
        typedef typename std::underlying_type<EnumType>::type UnderlyingType;

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