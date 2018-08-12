#pragma once

#include <limits>
#include <type_traits>
#include <tuple>

#include "Type.h"

namespace Chroma
{
    namespace detail
    {
        template<class NumericT, template<NumericT> class IteratorT, NumericT max, NumericT min, NumericT index>
        struct IterateRangeImplementation
        {
            template<class... Args>
            static void Forward(Args&& ... args)
            {
                static_assert(max >= min, "Max must be greater than or equal to min when you iterate over a range.");
                IterateRangeImplementation<NumericT, IteratorT, max, min, index - 1>::Forward(std::forward<Args>(args)...);
                IteratorT<index>::Do(std::forward<Args>(args)...);
            }

            template<class... Args>
            static void Backward(Args&& ... args)
            {
                static_assert(max >= min, "Max must be greater than or equal to min when you iterate over a range.");
                IteratorT<index>::Do(std::forward<Args>(args)...);
                IterateRangeImplementation<NumericT, IteratorT, max, min, index - 1>::Backward(std::forward<Args>(args)...);
            }

            template<class ValidityT, class... Args>
            static ValidityT CheckStopForward(ValidityT invalid, Args&& ... args)
            {
                static_assert(max >= min, "Max must be greater than or equal to min when you iterate over a range.");
                if (IterateRangeImplementation<NumericT, IteratorT, max, min, index - 1>::CheckStopForward<ValidityT>(invalid, std::forward<Args>(args)...) == invalid)
                    return invalid;

                return IteratorT<index>::Check(std::forward<Args>(args)...);
            }

            template<class ValidityT, class... Args>
            static ValidityT CheckStopBackward(ValidityT invalid, Args&& ... args)
            {
                static_assert(max >= min, "Max must be greater than or equal to min when you iterate over a range.");
                if (IteratorT<index>::Check(std::forward<Args>(args)...) == invalid)
                    return invalid;

                return IterateRangeImplementation<NumericT, IteratorT, max, min, index - 1>::CheckStopBackward<ValidityT>(invalid, std::forward<Args>(args)...);
            }
        };

        template<class NumericT, template<NumericT> class IteratorT, NumericT max, NumericT min>
        struct IterateRangeImplementation<NumericT, IteratorT, max, min, min>
        {
            template<class... Args>
            static void Forward(Args&& ... args)
            {
                static_assert(max >= min, "Max must be greater than or equal to min when you iterate over a range.");
                IteratorT<min>::Do(std::forward<Args>(args)...);
            }

            template<class... Args>
            static void Backward(Args&& ... args)
            {
                static_assert(max >= min, "Max must be greater than or equal to min when you iterate over a range.");
                IteratorT<min>::Do(std::forward<Args>(args)...);
            }

            template<class ValidityT, class... Args>
            static ValidityT CheckStopForward(ValidityT invalid, Args&& ... args)
            {
                static_assert(max >= min, "Max must be greater than or equal to min when you iterate over a range.");
                return IteratorT<min>::Check(std::forward<Args>(args)...);
            }

            template<class ValidityT, class... Args>
            static ValidityT CheckStopBackward(ValidityT invalid, Args&& ... args)
            {
                static_assert(max >= min, "Max must be greater than or equal to min when you iterate over a range.");
                return IteratorT<min>::Check(std::forward<Args>(args)...);
            }
        };

        template<class NumericT, template<NumericT> class IteratorT, NumericT max, NumericT min, NumericT index>
        struct IterateRangeImplementationEnum
        {
            typedef typename std::underlying_type<NumericT>::type Underlying;

            template<class... Args>
            static void Forward(Args&& ... args)
            {
                static_assert(static_cast<Underlying>(max) >= static_cast<Underlying>(min), "Max must be greater than or equal to min when you iterate over a range.");
                IterateRangeImplementationEnum<NumericT, IteratorT, max, min, static_cast<NumericT>((static_cast<Underlying>(index) - 1))>::Forward(std::forward<Args>(args)...);
                IteratorT<index>::Do(std::forward<Args>(args)...);
            }

            template<class... Args>
            static void Backward(Args&& ... args)
            {
                static_assert(static_cast<Underlying>(max) >= static_cast<Underlying>(min), "Max must be greater than or equal to min when you iterate over a range.");
                IteratorT<index>::Do(std::forward<Args>(args)...);
                IterateRangeImplementationEnum<NumericT, IteratorT, max, min, static_cast<NumericT>((static_cast<Underlying>(index) - 1))>::Backward(std::forward<Args>(args)...);
            }

            template<class ValidityT, class... Args>
            static ValidityT CheckStopForward(ValidityT invalid, Args&& ... args)
            {
                static_assert(static_cast<Underlying>(max) >= static_cast<Underlying>(min), "Max must be greater than or equal to min when you iterate over a range.");
                if (IterateRangeImplementationEnum<NumericT, IteratorT, max, min, static_cast<NumericT>((static_cast<Underlying>(index) - 1))>::CheckStopForward<ValidityT>(invalid, std::forward<Args>(args)...) == invalid)
                    return invalid;

                return IteratorT<min>::Check(std::forward<Args>(args)...);
            }

            template<class ValidityT, class... Args>
            static ValidityT CheckStopBackward(ValidityT invalid, Args&& ... args)
            {
                static_assert(static_cast<Underlying>(max) >= static_cast<Underlying>(min), "Max must be greater than or equal to min when you iterate over a range.");
                if (IteratorT<min>::Check(std::forward<Args>(args)...) == invalid)
                    return invalid;

                return IterateRangeImplementationEnum<NumericT, IteratorT, max, min, static_cast<NumericT>((static_cast<Underlying>(index) - 1))>::CheckStopBackward<ValidityT>(invalid, std::forward<Args>(args)...);
            }
        };

        template<class NumericT, template<NumericT> class IteratorT, NumericT max, NumericT min>
        struct IterateRangeImplementationEnum<NumericT, IteratorT, max, min, min>
        {
            typedef typename std::underlying_type<NumericT>::type Underlying;

            template<class... Args>
            static void Forward(Args&& ... args)
            {
                static_assert(static_cast<Underlying>(max) >= static_cast<Underlying>(min), "Max must be greater than or equal to min when you iterate over a range.");
                IteratorT<min>::Do(std::forward<Args>(args)...);
            }

            template<class... Args>
            static void Backward(Args&& ... args)
            {
                static_assert(static_cast<Underlying>(max) >= static_cast<Underlying>(min), "Max must be greater than or equal to min when you iterate over a range.");
                IteratorT<min>::Do(std::forward<Args>(args)...);
            }

            template<class ValidityT, class... Args>
            static ValidityT CheckStopForward(ValidityT invalid, Args&& ... args)
            {
                static_assert(static_cast<Underlying>(max) >= static_cast<Underlying>(min), "Max must be greater than or equal to min when you iterate over a range.");
                return IteratorT<min>::Check(std::forward<Args>(args)...);
            }

            template<class ValidityT, class... Args>
            static ValidityT CheckStopBackward(ValidityT invalid, Args&& ... args)
            {
                static_assert(static_cast<Underlying>(max) >= static_cast<Underlying>(min), "Max must be greater than or equal to min when you iterate over a range.");
                return IteratorT<min>::Check(std::forward<Args>(args)...);
            }
        };

        template<bool done, size_t upper, size_t count>
        struct Filler
        {
            template<class... Args, class... PassArgs>
            static void Do(std::tuple<Args...>& tuple, std::tuple<PassArgs...>&& passTuple)
            {
                std::get<count>(tuple) = std::get<count>(passTuple);
                Filler<count + 1 == upper, upper, count + 1>::Do(tuple, std::move(passTuple));
            }
        };

        template<size_t upper, size_t count>
        struct Filler<true, upper, count>
        {
            template<class... Args, class... PassArgs>
            static void Do(std::tuple<Args...>& tuple, std::tuple<PassArgs...>&& passTuple)
            {
                std::get<count>(tuple) = std::get<count>(passTuple);
            }
        };
    }

    // IteratorT needs to have a static Do function
    // You can dispatch to different Do functions in the same IteratorT by overloading
    // Range is [min, max]
    template<class NumericT, template<NumericT> class IteratorT, NumericT max, NumericT min = std::numeric_limits<NumericT>::min(), class... Args, typename std::enable_if<!std::is_enum<NumericT>::value, int>::type = 0>
    void IterateRangeForward(Args&& ... args)
    {
        static_assert(max >= min, "Max must be greater than or equal to min when you iterate over a range.");
        detail::IterateRangeImplementation<NumericT, IteratorT, max, min, max>::Forward(std::forward<Args>(args)...);
    }

    // IteratorT needs to have a static Do function
    // You can dispatch to different Do functions in the same IteratorT by overloading
    // Range is [min, max]
    template<class NumericT, template<NumericT> class IteratorT, NumericT max, NumericT min = std::numeric_limits<NumericT>::min(), class... Args, typename std::enable_if<!std::is_enum<NumericT>::value, int>::type = 0>
    void IterateRangeBackward(Args&& ... args)
    {
        static_assert(max >= min, "Max must be greater than or equal to min when you iterate over a range.");
        detail::IterateRangeImplementation<NumericT, IteratorT, max, min, max>::Backward(std::forward<Args>(args)...);
    }

    // IteratorT needs to have a static Do function
    // You can dispatch to different Do functions in the same IteratorT by overloading
    // Range is [min, max]
    template<class NumericT, template<NumericT> class IteratorT, NumericT max, NumericT min = std::numeric_limits<NumericT>::min(), class... Args, typename std::enable_if<!std::is_enum<NumericT>::value, int>::type = 0>
    void IterateRange(Args&& ... args)
    {
        IterateRangeForward<NumericT, IteratorT, max, min>(std::forward<Args>(args)...);
    }

    // IteratorT needs to have a static Check function
    // You can dispatch to different Check functions in the same IteratorT by overloading
    // Range is [min, max]
    // Invalid value is hit: returns invalid. Gets all the way through: returns the last valid value returned by IteratorT
    template<class NumericT, template<NumericT> class IteratorT, class ValidityT, NumericT max, NumericT min = std::numeric_limits<NumericT>::min(), class... Args, typename std::enable_if<!std::is_enum<NumericT>::value, int>::type = 0>
    ValidityT IterateRangeForwardCheckStop(ValidityT stop, Args&& ... args)
    {
        static_assert(max >= min, "Max must be greater than or equal to min when you iterate over a range.");
        return detail::IterateRangeImplementation<NumericT, IteratorT, max, min, max>::CheckStopForward<ValidityT>(stop, std::forward<Args>(args)...);
    }

    // IteratorT needs to have a static Check function
    // You can dispatch to different Check functions in the same IteratorT by overloading
    // Range is [min, max]
    // Invalid value is hit: returns invalid. Gets all the way through: returns the last valid value returned by IteratorT
    template<class NumericT, template<NumericT> class IteratorT, class ValidityT, NumericT max, NumericT min = std::numeric_limits<NumericT>::min(), class... Args, typename std::enable_if<!std::is_enum<NumericT>::value, int>::type = 0>
    ValidityT IterateRangeBackwardCheckStop(ValidityT stop, Args&& ... args)
    {
        static_assert(max >= min, "Max must be greater than or equal to min when you iterate over a range.");
        return detail::IterateRangeImplementation<NumericT, IteratorT, max, min, max>::CheckStopBackward<ValidityT>(stop, std::forward<Args>(args)...);
    }

    // IteratorT needs to have a static Check function
    // You can dispatch to different Check functions in the same IteratorT by overloading
    // Range is [min, max]
    // Invalid value is hit: returns invalid. Gets all the way through: returns the last valid value returned by IteratorT
    template<class NumericT, template<NumericT> class IteratorT, class ValidityT, NumericT max, NumericT min = std::numeric_limits<NumericT>::min(), class... Args, typename std::enable_if<!std::is_enum<NumericT>::value, int>::type = 0>
    ValidityT IterateRangeCheckStop(ValidityT stop, Args&& ... args)
    {
        return IterateRangeForwardCheckStop<NumericT, IteratorT, ValidityT, max, min>(stop, std::forward<Args>(args)...);
    }

    // IteratorT needs to have a static Do function
    // You can dispatch to different Do functions in the same IteratorT by overloading
    // Range is [min, max]
    template<class NumericT, template<NumericT> class IteratorT, NumericT max, NumericT min = std::numeric_limits<NumericT>::min(), class... Args, typename std::enable_if<std::is_enum<NumericT>::value, int>::type = 0>
    void IterateRangeForward(Args&& ... args)
    {
        typedef typename std::underlying_type<NumericT>::type Underlying;
        static_assert(static_cast<Underlying>(max) >= static_cast<Underlying>(min), "Max must be greater than or equal to min when you iterate over a range.");
        detail::IterateRangeImplementationEnum<NumericT, IteratorT, max, min, max>::Forward(std::forward<Args>(args)...);
    }

    // IteratorT needs to have a static Do function
    // You can dispatch to different Do functions in the same IteratorT by overloading
    // Range is [min, max]
    template<class NumericT, template<NumericT> class IteratorT, NumericT max, NumericT min = std::numeric_limits<NumericT>::min(), class... Args, typename std::enable_if<std::is_enum<NumericT>::value, int>::type = 0>
    void IterateRangeBackward(Args&& ... args)
    {
        typedef typename std::underlying_type<NumericT>::type Underlying;
        static_assert(static_cast<Underlying>(max) >= static_cast<Underlying>(min), "Max must be greater than or equal to min when you iterate over a range.");
        detail::IterateRangeImplementationEnum<NumericT, IteratorT, max, min, max>::Backward(std::forward<Args>(args)...);
    }

    // IteratorT needs to have a static Do function
    // You can dispatch to different Do functions in the same IteratorT by overloading
    // Range is [min, max]
    template<class NumericT, template<NumericT> class IteratorT, NumericT max, NumericT min = std::numeric_limits<NumericT>::min(), class... Args, typename std::enable_if<std::is_enum<NumericT>::value, int>::type = 0>
    void IterateRange(Args&& ... args)
    {
        IterateRangeForward<NumericT, IteratorT, max, min>(std::forward<Args>(args)...);
    }

    // IteratorT needs to have a static Check function
    // You can dispatch to different Check functions in the same IteratorT by overloading
    // Range is [min, max]
    // Invalid value is hit: returns invalid. Gets all the way through: returns the last valid value returned by IteratorT
    template<class NumericT, template<NumericT> class IteratorT, class ValidityT, NumericT max, NumericT min = std::numeric_limits<NumericT>::min(), class... Args, typename std::enable_if<std::is_enum<NumericT>::value, int>::type = 0>
    ValidityT IterateRangeForwardCheckStop(ValidityT stop, Args&& ... args)
    {
        typedef typename std::underlying_type<NumericT>::type Underlying;
        static_assert(static_cast<Underlying>(max) >= static_cast<Underlying>(min), "Max must be greater than or equal to min when you iterate over a range.");
        return detail::IterateRangeImplementationEnum<NumericT, IteratorT, max, min, max>::CheckStopForward<ValidityT>(stop, std::forward<Args>(args)...);
    }

    // IteratorT needs to have a static Check function
    // You can dispatch to different Check functions in the same IteratorT by overloading
    // Range is [min, max]
    // Invalid value is hit: returns invalid. Gets all the way through: returns the last valid value returned by IteratorT
    template<class NumericT, template<NumericT> class IteratorT, class ValidityT, NumericT max, NumericT min = std::numeric_limits<NumericT>::min(), class... Args, typename std::enable_if<std::is_enum<NumericT>::value, int>::type = 0>
    ValidityT IterateRangeBackwardCheckStop(ValidityT stop, Args&& ... args)
    {
        typedef typename std::underlying_type<NumericT>::type Underlying;
        static_assert(static_cast<Underlying>(max) >= static_cast<Underlying>(min), "Max must be greater than or equal to min when you iterate over a range.");
        return detail::IterateRangeImplementationEnum<NumericT, IteratorT, max, min, max>::CheckStopBackward<ValidityT>(stop, std::forward<Args>(args)...);
    }

    // IteratorT needs to have a static Check function
    // You can dispatch to different Check functions in the same IteratorT by overloading
    // Range is [min, max]
    // Invalid value is hit: returns invalid. Gets all the way through: returns the last valid value returned by IteratorT
    template<class NumericT, template<NumericT> class IteratorT, class ValidityT, NumericT max, NumericT min = std::numeric_limits<NumericT>::min(), class... Args, typename std::enable_if<std::is_enum<NumericT>::value, int>::type = 0>
    ValidityT IterateRangeCheckStop(ValidityT stop, Args&& ... args)
    {
        return IterateRangeForwardCheckStop<NumericT, IteratorT, ValidityT, max, min>(stop, std::forward<Args>(args)...);
    }

    template<class... Args, class... PassArgs>
    void Fill(std::tuple<Args...>& tuple, PassArgs&& ... args)
    {
        static_assert(sizeof...(Args) == sizeof...(PassArgs), "The tuple needs to be the same size as the passed arguments.");
        detail::Filler<sizeof...(Args) - 1 == 0, sizeof...(Args) - 1, 0>::Do(tuple, std::tuple<PassArgs...>(std::forward<Args>(args)...));
    }
}