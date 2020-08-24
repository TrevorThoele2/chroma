#pragma once

#include <type_traits>

namespace Chroma
{
    namespace detail
    {
        template <class T, std::size_t = sizeof(T)>
        std::true_type is_complete_impl(T*);

        std::false_type is_complete_impl(...);
    }

    template <class T>
    using is_complete = decltype(detail::is_complete_impl(std::declval<T*>()));

    template<class T>
    static constexpr bool is_complete_v = is_complete<T>::value;
}