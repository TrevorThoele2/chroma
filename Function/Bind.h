#pragma once

#include <memory>
#include <exception>
#include "ParameterIndex.h"
#include "Generator.h"
#include "Iterate.h"

namespace function
{
    template<class... Args>
    class Binder
    {
    public:
        std::tuple<Args...> tuple;
        Binder(Args && ... args);
        Binder(std::tuple<Args...> &&tuple);
    };

    template<class... Args>
    Binder<Args...>::Binder(Args && ... args) : tuple(std::forward<Args>(args)...)
    {}

    template<class... Args>
    Binder<Args...>::Binder(std::tuple<Args...> &&tuple) : tuple(std::move(tuple))
    {}

    template<class... Args>
    Binder<Args...> Bind(Args && ... args)
    {
        return Binder<Args...>(std::forward<Args>(args)...);
    }

    template<class... Args>
    Binder<Args...> Bind(std::tuple<Args...> &&tuple)
    {
        return Binder<Args...>(std::move(tuple));
    }
}