#pragma once

#include "VariantTraits.h"

namespace Chroma
{
    template<class EnumT, EnumT selector>
    struct StrategySelector {};

    namespace detail
    {
        template<class VariantT, class ExecutionObject>
        class VariantSwitch
        {
        private:
            typedef typename VariantT::VariadicTemplateT VariadicTemplateT;
        public:
            // Normal
            template<class... Args>
            static void Do(VariantT& variant, Args&& ... args)
            {
                typedef Impl<VariantT::count> Impl;
                Impl::Do(variant, std::forward<Args>(args)...);
            }

            // Normal
            template<class Ret, class... Args>
            static Ret DoReturn(VariantT& variant, Args&& ... args)
            {
                typedef Impl<VariantT::count> Impl;
                return Impl::template DoReturn<Ret>(variant, std::forward<Args>(args)...);
            }

            // Same
            template<class... Args>
            static void DoSame(VariantT& variant1, VariantT& variant2, Args&& ... args)
            {
                typedef Impl<VariantT::count> Impl;
                Impl::DoSame(variant1, variant2, std::forward<Args>(args)...);
            }

            // Same
            template<class Ret, class... Args>
            static Ret DoSameReturn(VariantT& variant1, VariantT& variant2, Args&& ... args)
            {
                typedef Impl<VariantT::count> Impl;
                return Impl::template DoSameReturn<Ret>(variant1, variant2, std::forward<Args>(args)...);
            }
        private:
            // Normal
            template<class T, class... Args>
            static void DoUltimately(VariantT& variant, Args&& ... args)
            {
                ExecutionObject::Do(variant.template Get<T>(), std::forward<Args>(args)...);
            }

            // Normal
            template<class T, class Ret, class... Args>
            static Ret DoUltimatelyReturn(VariantT& variant, Args&& ... args)
            {
                return ExecutionObject::DoReturn(variant.template Get<T>(), std::forward<Args>(args)...);
            }

            // Normal
            template<class T, class... Args>
            static void DoSameUltimately(VariantT& variant1, VariantT& variant2, Args&& ... args)
            {
                ExecutionObject::Do(variant1.template Get<T>(), variant2.template Get<T>(), std::forward<Args>(args)...);
            }

            // Normal
            template<class T, class Ret, class... Args>
            static Ret DoSameUltimatelyReturn(VariantT& variant1, VariantT& variant2, Args&& ... args)
            {
                return ExecutionObject::DoReturn(variant1.template Get<T>(), variant2.template Get<T>(), std::forward<Args>(args)...);
            }

            template<VariadicTemplateSize index>
            struct Impl
            {
                typedef typename VariadicTemplateT::template Parameter<index - 1>::Type PieceType;

                // Normal
                template<class... Args>
                static void Do(VariantT& variant, Args&& ... args)
                {
                    if (variant.template Is<PieceType>())
                        DoUltimately<PieceType>(variant, std::forward<Args>(args)...);
                    else
                        Impl<index - 1>::Do(variant, std::forward<Args>(args)...);
                }

                // Normal
                template<class Ret, class... Args>
                static Ret DoReturn(VariantT& variant, Args&& ... args)
                {
                    if (variant.template Is<PieceType>())
                        return DoUltimatelyReturn<PieceType, Ret>(variant, std::forward<Args>(args)...);
                    else
                        return Impl<index - 1>::template DoReturn<Ret>(variant, std::forward<Args>(args)...);
                }

                // Same
                template<class... Args>
                static void DoSame(VariantT& variant1, VariantT& variant2, Args&& ... args)
                {
                    if (variant1.template Is<PieceType>() && variant2.template Is<PieceType>())
                        DoSameUltimately<PieceType>(variant1, variant2, std::forward<Args>(args)...);
                    else
                        Impl<index - 1>::DoSame(variant1, variant2, std::forward<Args>(args)...);
                }

                // Same
                template<class Ret, class... Args>
                static Ret DoSameReturn(VariantT& variant1, VariantT& variant2, Args&& ... args)
                {
                    if (variant1.template Is<PieceType>() && variant2.template Is<PieceType>())
                        return DoSameUltimatelyReturn<PieceType, Ret>(variant1, variant2, std::forward<Args>(args)...);
                    else
                        return Impl<index - 1>::template DoSameReturn<Ret>(variant1, variant2, std::forward<Args>(args)...);
                }
            };

            template<>
            struct Impl<0>
            {
                typedef typename VariadicTemplateT::template Parameter<0>::Type PieceType;

                // Normal
                template<class... Args>
                static void Do(VariantT& variant, Args&& ... args)
                {
                    DoUltimately<PieceType>(variant, std::forward<Args>(args)...);
                }

                // Normal
                template<class Ret, class... Args>
                static Ret DoReturn(VariantT& variant, Args&& ... args)
                {
                    return DoUltimatelyReturn<PieceType, Ret>(variant, std::forward<Args>(args)...);
                }

                // Same
                template<class... Args>
                static void DoSame(VariantT& variant1, VariantT& variant2, Args&& ... args)
                {
                    DoSameUltimately<PieceType>(variant1, variant2, std::forward<Args>(args)...);
                }

                // Same
                template<class Ret, class... Args>
                static Ret DoSameReturn(VariantT& variant1, VariantT& variant2, Args&& ... args)
                {
                    return DoSameUltimatelyReturn<PieceType, Ret>(variant1, variant2, std::forward<Args>(args)...);
                }
            };
        };

        template<class Visitor>
        class MultipleVisitImplementation
        {
        public:
            template<class T, class Variant2, class... Args>
            static void Do(T& t, Variant2& variant2, Args&& ... args);
        };

        template<class Visitor, class Ret>
        class MultipleVisitReturnImplementation
        {
        public:
            template<class T, class Variant2, class... Args>
            static Ret DoReturn(T& t, Variant2& variant2, Args&& ... args);
        };
    }

    // The Visitor needs to have a static Do function with (Type, Args...) arguments
    template<class Visitor, class VariantType, class... Args>
    static void Visit(VariantType& variant, Args&& ... args)
    {
        static_assert(VariantTraits<VariantType>::isVariant, "The variant argument must actually be a Variant.");
        detail::VariantSwitch<VariantType, Visitor>::Do(variant, std::forward<Args>(args)...);
    }

    // The Visitor needs to have a static DoReturn function with (Type, Args...) arguments
    template<class Visitor, class Ret, class VariantType, class... Args>
    static Ret VisitReturn(VariantType& variant, Args&& ... args)
    {
        static_assert(VariantTraits<VariantType>::isVariant, "The variant argument must actually be a Variant.");
        return detail::VariantSwitch<VariantType, Visitor>::template DoReturn<Ret>(variant, std::forward<Args>(args)...);
    }

    // The Visitor needs to have a static Do function with (Type, Args...) arguments
    template<class Visitor, class Variant1, class Variant2, class... Args>
    static void VisitMultiple(Variant1& variant1, Variant2& variant2, Args&& ... args)
    {
        static_assert(VariantTraits<Variant1>::isVariant && VariantTraits<Variant2>::isVariant, "The variant arguments must actually be a Variant.");
        detail::VariantSwitch<Variant2, detail::MultipleVisitImplementation<Visitor>>::Do(variant2, variant1, std::forward<Args>(args)...);
    }

    // The Visitor needs to have a static DoReturn function with (Type, Args...) arguments
    template<class Visitor, class Ret, class Variant1, class Variant2, class... Args>
    static Ret VisitReturnMultiple(Variant1& variant1, Variant2& variant2, Args&& ... args)
    {
        static_assert(VariantTraits<Variant1>::isVariant && VariantTraits<Variant2>::isVariant, "The variant arguments must actually be a Variant.");
        return detail::VariantSwitch<Variant2, detail::MultipleVisitReturnImplementation<Visitor, Ret>>::template DoReturn<Ret>(variant2, variant1, std::forward<Args>(args)...);
    }

    // The Visitor needs to have a static Do function with (Type, Args...) arguments
    template<class Visitor, class VariantType, class... Args>
    static void VisitMultipleSame(VariantType& variant1, VariantType& variant2, Args&& ... args)
    {
        detail::VariantSwitch<VariantType, Visitor>::DoSame(variant1, variant2, std::forward<Args>(args)...);
    }

    // The Visitor needs to have a static DoReturn function with (Type, Args...) arguments
    template<class Visitor, class Ret, class VariantType, class... Args>
    static Ret VisitReturnMultipleSame(VariantType& variant1, VariantType& variant2, Args&& ... args)
    {
        return detail::VariantSwitch<VariantType, Visitor>::template DoSameReturn<Ret>(variant1, variant2, std::forward<Args>(args)...);
    }
}