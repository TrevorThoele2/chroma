#pragma once

#include "VariadicTemplate.h"

namespace Chroma
{
    template<class Value, class OwningClass, Value OwningClass::*fieldPass>
    class ClassPathPiece
    {
    public:
        typedef Value ValueT;
        typedef OwningClass OwningClassT;
        static constexpr ValueT OwningClassT::*field = fieldPass;
    };

    // Describes a path through a class heirarchy
    template<class... Pieces>
    class ClassPath
    {
    private:
        typedef ::function::VariadicTemplate<Pieces...> VariadicTemplateT;
    public:
        typedef typename VariadicTemplateT::Parameter<0>::Type::ValueT ValueRet;
        typedef typename VariadicTemplateT::Parameter<0>::Type::OwningClassT FinalOwningRet;
        typedef typename VariadicTemplateT::Parameter<VariadicTemplateT::count - 1>::Type::OwningClassT OwningClassSend;
    private:
        template<size_t index>
        struct Iterator
        {
            typedef typename VariadicTemplateT::Parameter<index>::Type PieceT;
            typedef typename PieceT::ValueT ValueT;
            typedef typename PieceT::OwningClassT OwningClassT;
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
            typedef typename VariadicTemplateT::Parameter<0>::Type PieceT;
            typedef typename PieceT::ValueT ValueT;
            typedef typename PieceT::OwningClassT OwningClassT;
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