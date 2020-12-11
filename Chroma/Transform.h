#pragma once

#include "VariadicTemplate.h"
#include "VariadicNonTypeTemplate.h"

namespace Chroma
{
    // Transforms a parameter pack according to the trait passed
    // The transformer needs to have a type called Type
    template<class Variadic, template<class, VariadicTemplateSize> class Transformer>
    class TransformToNonTypeWithIndex
    {
    private:
        template<VariadicTemplateSize index, auto... HoldArgs>
        struct Impl
        {
            static constexpr auto pieceNonType = Transformer<typename Variadic::template Parameter<index - 1>::Type, index - 1>::nonType;
            using Type = typename Impl<index - 1, pieceNonType, HoldArgs...>::Type;
        };

        template<auto... HoldArgs>
        struct Impl<0, HoldArgs...>
        {
            using Type = VariadicNonTypeTemplate<HoldArgs...>;
        };
    public:
        using Type = typename Impl<Variadic::count>::Type;
    };

    template<class Variadic, template<class, VariadicTemplateSize> class Transformer>
    using to_non_type_with_index = typename TransformToNonTypeWithIndex<Variadic, Transformer>::Type;
}