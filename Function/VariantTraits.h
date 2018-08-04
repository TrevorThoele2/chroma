#pragma once

namespace function
{
    template<class... Args>
    class Variant;

    template<class T>
    struct VariantTraits
    {
        static constexpr bool isVariant = false;
    };

    template<class... Args>
    struct VariantTraits<Variant<Args...>>
    {
        static constexpr bool isVariant = true;
    };

    template<class... Args>
    struct VariantTraits<const Variant<Args...>>
    {
        static constexpr bool isVariant = true;
    };

    template<class... Args>
    struct VariantTraits<Variant<Args...>&>
    {
        static constexpr bool isVariant = true;
    };

    template<class... Args>
    struct VariantTraits<const Variant<Args...>&>
    {
        static constexpr bool isVariant = true;
    };

    template<class... Args>
    struct VariantTraits<Variant<Args...>&&>
    {
        static constexpr bool isVariant = true;
    };
}