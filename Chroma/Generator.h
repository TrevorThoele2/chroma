#pragma once

#include "RangeSize.h"
#include "Indices.h"

namespace Chroma
{
    template<class IterateT, template<IterateT...> class GenT>
    class TypeGeneratorRange
    {
    private:
        template<bool end, bool backward, bool includeEnd, IterateT lower, IterateT upper, IterateT count, IterateT... HoldArgs>
        struct Impl
        {
            static_assert(lower <= upper, "Lower must be less than or equal to upper.");
            typedef typename Impl<(includeEnd) ? ((backward) ? count == lower : count == upper) : ((backward) ? IterateT(count - 1) == lower : IterateT(count + 1) == upper), backward, includeEnd, lower, upper, (backward) ? IterateT(count - 1) : IterateT(count + 1), HoldArgs..., count>::T T;
        };

        template<bool backward, bool includeEnd, IterateT lower, IterateT upper, IterateT count, IterateT... HoldArgs>
        struct Impl<true, backward, includeEnd, lower, upper, count, HoldArgs...>
        {
            static_assert(lower <= upper, "Lower must be less than or equal to upper.");
            typedef GenT<HoldArgs...> T;
        };
    public:
        template<IterateT lower, IterateT upper, bool includeEnd = false>
        using Backward = typename Impl<upper == lower, true, includeEnd, lower, upper, upper>::T;
        template<IterateT lower, IterateT upper, bool includeEnd = false>
        using Forward = typename Impl<upper == lower, false, includeEnd, lower, upper, lower>::T;
        template<IterateT left, IterateT right, bool includeEnd = false>
        using Auto = typename Impl<right == left, (left >= right), includeEnd, (left >= right) ? right : left, (left >= right) ? left : right, (left >= right) ? left : right>::T;
    };

    // IteratorT needs a type called Type in it
    template<class IterateT, template<class...> class GenT, template<IterateT> class IteratorT>
    class TypeGeneratorRangeIterate
    {
    private:
        template<bool end, bool backward, bool includeEnd, IterateT lower, IterateT upper, IterateT count, class... HoldArgs>
        struct Impl
        {
            static_assert(lower <= upper, "Lower must be less than or equal to upper.");
            typedef typename IteratorT<count>::Type PieceType;
            typedef typename Impl<(includeEnd) ? ((backward) ? count == lower : count == upper) : ((backward) ? IterateT(count - 1) == lower : IterateT(count + 1) == upper), backward, includeEnd, lower, upper, (backward) ? IterateT(count - 1) : IterateT(count + 1), HoldArgs..., PieceType>::T T;
        };

        template<bool backward, bool includeEnd, IterateT lower, IterateT upper, IterateT count, class... HoldArgs>
        struct Impl<true, backward, includeEnd, lower, upper, count, HoldArgs...>
        {
            static_assert(lower <= upper, "Lower must be less than or equal to upper.");
            typedef GenT<HoldArgs...> T;
        };
    public:
        template<IterateT lower, IterateT upper, bool includeEnd = false>
        using Backward = typename Impl<upper == lower, true, includeEnd, lower, upper, upper>::T;
        template<IterateT lower, IterateT upper, bool includeEnd = false>
        using Forward = typename Impl<upper == lower, false, includeEnd, lower, upper, lower>::T;
        template<IterateT left, IterateT right, bool includeEnd = false>
        using Auto = typename Impl<right == left, (left >= right), includeEnd, (left >= right) ? right : left, (left >= right) ? left : right, (left >= right) ? left : right>::T;
    };
}