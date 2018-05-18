#pragma once

namespace function
{
    template<class Value, class Exp>
    Value Power(Value value, Exp exp)
    {
        Value ret(1);
        while(exp-- > 0)
            ret *= value;

        return ret;
    }

    namespace detail
    {
        template<class Left, class Right, Left left, Right right>
        struct CompilePower
        {
            constexpr static Left value = left * CompilePower<left, right - 1>::value;
        };

        template<class Left, class Right, Left left>
        struct CompilePower<Left, Right, left, 0>
        {
            constexpr static Left value = 1;
        };
    }

    template<class Left, class Right, Left left, Right right>
    constexpr Left CompilePower()
    {
        return detail::CompilePower<Left, Right, left, right>();
    }
}