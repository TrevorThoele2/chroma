#pragma once

namespace Chroma
{
    template<class EnumT, EnumT selector>
    struct StrategySelector {};

    namespace detail
    {
        template<class VariantT, class Use>
        class VariantSwitch
        {
        private:
            typedef typename VariantT::VariadicTemplateT VariadicTemplateT;
        public:
            // Normal
            template<class... Args>
            static void Do(VariantT& variant, Args&& ... args)
            {
                Impl<VariantT::count>::Do(variant, std::forward<Args>(args)...);
            }

            // Normal
            template<class Ret, class... Args>
            static Ret DoReturn(VariantT& variant, Args&& ... args)
            {
                return Impl<VariantT::count>::DoReturn<Ret>(variant, std::forward<Args>(args)...);
            }

            // Same
            template<class... Args>
            static void DoSame(VariantT& variant1, VariantT& variant2, Args&& ... args)
            {
                Impl<VariantT::count>::DoSame(variant1, variant2, std::forward<Args>(args)...);
            }

            // Same
            template<class Ret, class... Args>
            static Ret DoSameReturn(VariantT& variant1, VariantT& variant2, Args&& ... args)
            {
                return Impl<VariantT::count>::DoSameReturn<Ret>(variant1, variant2, std::forward<Args>(args)...);
            }
        private:
            // Normal
            template<class T, class... Args>
            static void DoUltimately(VariantT& variant, Args&& ... args)
            {
                Use::Do(variant.Get<T>(), std::forward<Args>(args)...);
            }

            // Normal
            template<class T, class Ret, class... Args>
            static Ret DoUltimatelyReturn(VariantT& variant, Args&& ... args)
            {
                return Use::DoReturn(variant.Get<T>(), std::forward<Args>(args)...);
            }

            // Normal
            template<class T, class... Args>
            static void DoSameUltimately(VariantT& variant1, VariantT& variant2, Args&& ... args)
            {
                Use::Do(variant1.Get<T>(), variant2.Get<T>(), std::forward<Args>(args)...);
            }

            // Normal
            template<class T, class Ret, class... Args>
            static Ret DoSameUltimatelyReturn(VariantT& variant1, VariantT& variant2, Args&& ... args)
            {
                return Use::DoReturn(variant1.Get<T>(), variant2.Get<T>(), std::forward<Args>(args)...);
            }

            template<VariadicTemplateSize index>
            struct Impl
            {
                typedef typename VariadicTemplateT::template Parameter<index - 1>::Type PieceType;

                // Normal
                template<class... Args>
                static void Do(VariantT& variant, Args&& ... args)
                {
                    if (variant.Is<PieceType>())
                        DoUltimately<PieceType>(variant, std::forward<Args>(args)...);
                    else
                        Impl<index - 1>::Do(variant, std::forward<Args>(args)...);
                }

                // Normal
                template<class Ret, class... Args>
                static Ret DoReturn(VariantT& variant, Args&& ... args)
                {
                    if (variant.Is<PieceType>())
                        return DoUltimatelyReturn<PieceType, Ret>(variant, std::forward<Args>(args)...);
                    else
                        return Impl<index - 1>::DoReturn<Ret>(variant, std::forward<Args>(args)...);
                }

                // Same
                template<class... Args>
                static void DoSame(VariantT& variant1, VariantT& variant2, Args&& ... args)
                {
                    if (variant1.Is<PieceType>() && variant2.Is<PieceType>())
                        DoSameUltimately<PieceType>(variant1, variant2, std::forward<Args>(args)...);
                    else
                        Impl<index - 1>::DoSame(variant1, variant2, std::forward<Args>(args)...);
                }

                // Same
                template<class Ret, class... Args>
                static Ret DoSameReturn(VariantT& variant1, VariantT& variant2, Args&& ... args)
                {
                    if (variant1.Is<PieceType>() && variant2.Is<PieceType>())
                        return DoSameUltimatelyReturn<PieceType, Ret>(variant1, variant2, std::forward<Args>(args)...);
                    else
                        return Impl<index - 1>::DoSameReturn<Ret>(variant1, variant2, std::forward<Args>(args)...);
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
    template<class Visitor, class Variant, class... Args>
    static void Visit(Variant& variant, Args&& ... args)
    {
        static_assert(VariantTraits<Variant>::isVariant, "The variant argument must actually be a Variant.");
        detail::VariantSwitch<Variant, Visitor>::Do(variant, std::forward<Args>(args)...);
    }

    // The Visitor needs to have a static DoReturn function with (Type, Args...) arguments
    template<class Visitor, class Ret, class Variant, class... Args>
    static Ret VisitReturn(Variant& variant, Args&& ... args)
    {
        static_assert(VariantTraits<Variant>::isVariant, "The variant argument must actually be a Variant.");
        return detail::VariantSwitch<Variant, Visitor>::DoReturn<Ret>(variant, std::forward<Args>(args)...);
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
        return detail::VariantSwitch<Variant2, detail::MultipleVisitReturnImplementation<Visitor, Ret>>::DoReturn<Ret>(variant2, variant1, std::forward<Args>(args)...);
    }

    // The Visitor needs to have a static Do function with (Type, Args...) arguments
    template<class Visitor, class Variant, class... Args>
    static void VisitMultipleSame(Variant& variant1, Variant& variant2, Args&& ... args)
    {
        detail::VariantSwitch<Variant, Visitor>::DoSame(variant1, variant2, std::forward<Args>(args)...);
    }

    // The Visitor needs to have a static DoReturn function with (Type, Args...) arguments
    template<class Visitor, class Ret, class Variant, class... Args>
    static Ret VisitReturnMultipleSame(Variant& variant1, Variant& variant2, Args&& ... args)
    {
        return detail::VariantSwitch<Variant, Visitor>::DoSameReturn<Ret>(variant1, variant2, std::forward<Args>(args)...);
    }
}