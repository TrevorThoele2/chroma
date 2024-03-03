#pragma once

#include <utility>

namespace Chroma
{
    namespace detail
    {
        template<class T, class... Args>
        auto is_braces_constructible_impl(int) -> decltype(void(T{std::declval<Args>()...}), std::true_type())
        {
            return {};
        }

        template<class T, class... Args>
        auto is_braces_constructible_impl(...) -> std::false_type
        {
            return {};
        }
    }

    template<class T, class... Args>
    struct is_braces_constructible : decltype(detail::is_braces_constructible_impl<T, Args...>(0))
    {};

    template<class T, class... Args>
    static constexpr bool is_braces_constructible_v = is_braces_constructible<T, Args...>::value;

    template<class T>
    struct is_braces_default_constructible : is_braces_constructible<T>
    {};

    template<class T>
    static constexpr bool is_braces_default_constructible_v = is_braces_constructible<T>::value;
}