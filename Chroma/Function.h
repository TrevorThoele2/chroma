#pragma once

#include <functional>

namespace Chroma
{
    template<class Ret, class... Args>
    std::function<Ret(Args...)> CreateFunction(Ret(*func)(Args...))
    {
        return std::function<Ret(Args...)>(func);
    }

    template<class Obj, class RealObj, class... Args>
    std::function<void(Args...)> CreateFunction(void(Obj::*func)(Args...), RealObj* obj)
    {
        return std::function<void(Args...)>([func, obj](Args && ... args) {(obj->*func)(std::forward<Args>(args)...); });
    }

    template<class Ret, class Obj, class RealObj, class... Args>
    std::function<Ret(Args...)> CreateFunction(Ret(Obj::*func)(Args...), RealObj* obj)
    {
        return std::function<Ret(Args...)>([func, obj](Args && ... args) { return (obj->*func)(std::forward<Args>(args)...); });
    }

    template<class Obj, class RealObj, class... Args>
    std::function<void(Args...)> CreateFunction(void(Obj::*func)(Args...), RealObj& obj)
    {
        return std::function<void(Args...)>([func, &obj](Args && ... args) { (obj.*func)(std::forward<Args>(args)...); });
    }

    template<class Ret, class Obj, class RealObj, class... Args>
    std::function<Ret(Args...)> CreateFunction(Ret(Obj::*func)(Args...), RealObj& obj)
    {
        return std::function<Ret(Args...)>([func, &obj](Args && ... args) { return (obj.*func)(std::forward<Args>(args)...); });
    }

    template<class Obj, class RealObj, class... Args>
    std::function<void(Args...)> CreateFunction(void(Obj::*func)(Args...) const, const RealObj* obj)
    {
        return std::function<void(Args...)>([func, obj](Args && ... args) {(obj->*func)(std::forward<Args>(args)...); });
    }

    template<class Ret, class Obj, class RealObj, class... Args>
    std::function<Ret(Args...)> CreateFunction(Ret(Obj::*func)(Args...) const, const RealObj* obj)
    {
        return std::function<Ret(Args...)>([func, obj](Args && ... args) { return (obj->*func)(std::forward<Args>(args)...); });
    }

    template<class Obj, class RealObj, class... Args>
    std::function<void(Args...)> CreateFunction(void(Obj::*func)(Args...) const, const RealObj& obj)
    {
        return std::function<void(Args...)>([func, obj](Args && ... args) {(obj.*func)(std::forward<Args>(args)...); });
    }

    template<class Ret, class Obj, class RealObj, class... Args, typename std::enable_if<!std::is_pointer<RealObj>::value, int>::type = 0>
    std::function<Ret(Args...)> CreateFunction(Ret(Obj::*func)(Args...) const, const RealObj& obj)
    {
        return std::function<Ret(Args...)>([func, &obj](Args && ... args) { return (obj.*func)(std::forward<Args>(args)...); });
    }
}