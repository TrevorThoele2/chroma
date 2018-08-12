#pragma once

#include <type_traits>
#include "Function.h"

namespace Chroma
{
    template<class LeftRet, class... LeftArgs, class RightRet, class... RightArgs, class... PassArgs, typename ::std::enable_if<!::std::is_same<LeftRet, void>::value && !::std::is_same<RightRet, void>::value, int>::type = 0>
    LeftRet Switch(std::true_type, const Function<LeftRet, LeftArgs...>& ifTrue, const Function<RightRet, RightArgs...>& ifFalse, PassArgs&& ... args)
    {
        return ifTrue(std::forward<PassArgs>(args)...);
    }

    template<class LeftRet, class... LeftArgs, class RightRet, class... RightArgs, class... PassArgs, typename ::std::enable_if<!::std::is_same<LeftRet, void>::value && !::std::is_same<RightRet, void>::value, int>::type = 0>
    RightRet Switch(std::false_type, const Function<LeftRet, LeftArgs...>& ifTrue, const Function<RightRet, RightArgs...>& ifFalse, PassArgs&& ... args)
    {
        return ifFalse(std::forward<PassArgs>(args)...);
    }

    template<class LeftRet, class... LeftArgs, class RightRet, class... RightArgs, class... PassArgs, typename ::std::enable_if<::std::is_same<LeftRet, void>::value || ::std::is_same<RightRet, void>::value, int>::type = 0>
    void Switch(std::true_type, const Function<LeftRet, LeftArgs...>& ifTrue, const Function<RightRet, RightArgs...>& ifFalse, PassArgs&& ... args)
    {
        ifTrue(std::forward<PassArgs>(args)...);
    }

    template<class LeftRet, class... LeftArgs, class RightRet, class... RightArgs, class... PassArgs, typename ::std::enable_if<::std::is_same<LeftRet, void>::value || ::std::is_same<RightRet, void>::value, int>::type = 0>
    void Switch(std::false_type, const Function<LeftRet, LeftArgs...>& ifTrue, const Function<RightRet, RightArgs...>& ifFalse, PassArgs&& ... args)
    {
        ifFalse(std::forward<PassArgs>(args)...);
    }
}