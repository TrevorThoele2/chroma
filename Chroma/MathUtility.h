#pragma once

#include <type_traits>

namespace Chroma
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
		template<class Left, class Right, Left left>
		class CompilePower
		{
		private:
			template<Right right>
			struct Impl
			{
				constexpr static Left value = left * Impl<right - 1>::value;
			};

			template<>
			struct Impl<0>
			{
				constexpr static Left value = 1;
			};
		public:
			template<Right right>
			constexpr static Left value = Impl<right>::value;
		};
	}

	template<class Left, class Right, Left left, Right right>
	constexpr Left CompilePower()
	{
		return detail::CompilePower<Left, Right, left>::template value<right>;
	}
}