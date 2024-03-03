#pragma once

#include "VariadicTemplate.h"

#include "Visit.h"

namespace function
{
    template<class... Args>
    class Variant;

    namespace detail
    {
        template<class Variant, class Use>
        class VariantSwitch;

        struct NullT
        {};

        template<class VariantT>
        class VariantIterationImplementation
        {
        private:
            typedef typename VariantT::VariadicTemplateT VariadicTemplateT;
            typedef VariadicTemplateSize ID;
            static const ID count = VariadicTemplateT::count;

            typedef std::type_index Type;

            template<VariadicTemplateSize index>
            struct IDStep
            {
                typedef typename VariadicTemplateT::template Parameter<index - 1>::Type PieceType;

                static Type GetCheckType()
                {
                    return typeid(PieceType);
                }

                static ID GetID(const VariantT& var)
                {
                    if (GetCheckType() == var.GetType())
                        return index - 1;
                    else
                        return IDStep<index - 1>::GetID(var);
                }

                static Type GetTypeIndex(ID id)
                {
                    if (id == index - 1)
                        return GetCheckType();
                    else
                        return IDStep<index - 1>::GetTypeIndex(id);
                }
            };

            template<>
            struct IDStep<0>
            {
                typedef typename VariadicTemplateT::template Parameter<0>::Type PieceType;

                static ID GetID(const VariantT& var)
                {
                    return 0;
                }

                static Type GetTypeIndex(ID id)
                {
                    return Type(typeid(PieceType));
                }
            };
        public:
            static ID GetID(const VariantT& var) { return IDStep<count>::GetID(var); }
            static Type GetTypeIndex(ID id) { return IDStep<count>::GetTypeIndex(id); }
        };

        enum class VariantVisitStrategy
        {
            DESTRUCTOR,
            EQUALITY,
            NON_EQUALITY,
            LESS,
            LESS_EQUAL,
            GREATER,
            GREATER_EQUAL,

            SET,
            TAKE_FROM,
            CONVERT_TO
        };

        class VariantVisitImplementation
        {
        public:
            template<class T, class... Args>
            static void Do(T& t, Variant<Args...> &variant, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::DESTRUCTOR>)
            {
                t.~T();
                variant.bytes.clear();
                variant.typeIndex = variant.NullType();
            }

            template<class T, class U>
            static bool DoReturn(const T& t, const U& u, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::EQUALITY>)
            {
                return t == u;
            }

            template<class T, class U>
            static bool DoReturn(const T& t, const U& u, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::NON_EQUALITY>)
            {
                return t != u;
            }

            template<class T, class U>
            static bool DoReturn(const T& t, const U& u, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::LESS>)
            {
                return t < u;
            }

            template<class T, class U>
            static bool DoReturn(const T& t, const U& u, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::LESS_EQUAL>)
            {
                return t <= u;
            }

            template<class T, class U>
            static bool DoReturn(const T& t, const U& u, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::GREATER>)
            {
                return t > u;
            }

            template<class T, class U>
            static bool DoReturn(const T& t, const U& u, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::GREATER_EQUAL>)
            {
                return t >= u;
            }

            template<class T, class... OtherArgs>
            static void Do(T& t, Variant<OtherArgs...>& setup, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::SET>)
            {
                setup.Set(t);
            }

            template<class T, class... OtherArgs>
            static void Do(T& t, Variant<OtherArgs...>& setup, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::TAKE_FROM>)
            {
                setup.Set(std::move(t));
            }

            template<class T, class... OtherArgs>
            static void Do(const T& t, Variant<OtherArgs...>& convertTo, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::CONVERT_TO>)
            {
                convertTo.Set(t);
            }

            template<class T, class... OtherArgs>
            static void Do(T&& t, Variant<OtherArgs...>& convertTo, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::CONVERT_TO>)
            {
                convertTo.Set(std::move(t));
            }
        };
    }
}