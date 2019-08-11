#pragma once

#include "VariadicTemplate.h"

namespace Chroma
{
    template<class Value, class OwningClass, Value OwningClass::*fieldPass>
    class ClassPathPiece
    {
    public:
        using ValueT = Value;
        using OwningClassT = OwningClass;
        static constexpr ValueT OwningClassT::*field = fieldPass;
    };

    // Describes a path through a class heirarchy
    template<class... Pieces>
    class ClassPath
    {
    private:
        using VariadicTemplateT = ::function::VariadicTemplate<Pieces...>;
    public:
        using ValueRet = typename VariadicTemplateT::Parameter<0>::Type::ValueT;
        using FinalOwningRet = typename VariadicTemplateT::Parameter<0>::Type::OwningClassT;
        using OwningClassSend = typename VariadicTemplateT::Parameter<VariadicTemplateT::count - 1>::Type::OwningClassT;
    private:
        template<size_t index>
        struct Iterator
        {
            using PieceT = typename VariadicTemplateT::Parameter<index>::Type;
            using ValueT = typename PieceT::ValueT;
            using OwningClassT = typename PieceT::OwningClassT;
            static constexpr ValueT OwningClassT::*field = PieceT::field;

            static ValueRet Retrieve(OwningClassT& owning)
            {
                return Iterator<index - 1>::Retrieve(owning.*field);
            }

            static FinalOwningRet& RetrieveFinalOwning(OwningClassT& owning)
            {
                return Iterator<index - 1>::RetrieveFinalOwning(owning.*field);
            }
        };

        template<>
        struct Iterator<0>
        {
            using PieceT = typename VariadicTemplateT::Parameter<0>::Type;
            using ValueT = typename PieceT::ValueT;
            using OwningClassT = typename PieceT::OwningClassT;
            static constexpr ValueT OwningClassT::*field = PieceT::field;

            static ValueT Retrieve(OwningClassT& owning)
            {
                return (owning.*field);
            }

            static FinalOwningRet& RetrieveFinalOwning(OwningClassT& owning)
            {
                return owning;
            }
        };
    public:
        static ValueRet Retrieve(OwningClassSend& owning)
        {
            return Iterator<VariadicTemplateT::count - 1>::Retrieve(owning);
        }

        static FinalOwningRet& RetrieveFinalOwning(OwningClassSend& owning)
        {
            return Iterator<VariadicTemplateT::count - 1>::RetrieveFinalOwning(owning);
        }
    };
}