#pragma once

#include <vector>
#include <list>
#include <functional>

#include "Function.h"

namespace Chroma
{
    template<class... Args>
    class EventConnection;

    template<class... Args>
    class Event
    {
    public:
        using FunctionT = std::function<void(Args...)>;
        using Connection = EventConnection<Args...>;
    public:
        Event();
        Event(const Event &arg);
        Event(Event &&arg) noexcept;
        ~Event();
        Event& operator=(const Event &arg);
        Event& operator=(Event &&arg) noexcept;

        void operator()(Args ... args);
        void Execute(Args ... args);

        template<class Ret>
        Connection Subscribe(Ret(*func)(Args...));
        template<class Ret, class Obj>
        Connection Subscribe(Ret(Obj::*func)(Args...), Obj& obj);
        template<class Ret, class Obj>
        Connection Subscribe(Ret(Obj::*func)(Args...) const, const Obj& obj);
        Connection Subscribe(const FunctionT& function);
        Connection Subscribe(FunctionT&& function);

        void Remove(Connection& remove);
        void Clear();
        [[nodiscard]] bool IsEmpty() const;
    private:
        using Slots = std::list<FunctionT>;
        using iterator = typename Slots::iterator;
        using const_iterator = typename Slots::const_iterator;

        Slots slots;
        // Used when executing this event
        iterator next;

        using Connections = std::list<Connection>;
        Connections connections;

        // Finds the offset of the next iterator
        [[nodiscard]] size_t FindNextOffset() const;
        // Sets the next iterator by an offset from the beginning of the list
        void SkipNext(size_t offset);

        void SetupConnectionsMove(Event &&arg);
        void SetupConnectionsCopy(const Event &arg);
    private:
        template<class... Args>
        friend class EventConnection;
    };

    template<class... Args>
    Event<Args...>::Event() : next(slots.end())
    {}

    template<class... Args>
    Event<Args...>::Event(const Event &arg) : slots(arg.slots)
    {
        next = slots.end();
        SetupConnectionsCopy(arg);
    }

    template<class... Args>
    Event<Args...>::Event(Event &&arg) noexcept : slots(std::move(arg.slots))
    {
        next = slots.end();
        SetupConnectionsMove(std::move(arg));
    }

    template<class... Args>
    Event<Args...>::~Event()
    {
        for (auto &loop : connections)
        {
            *loop.itr = slots.end();
            *loop.owner = nullptr;
        }
    }

    template<class... Args>
    Event<Args...>& Event<Args...>::operator=(const Event &arg)
    {
        const size_t endID = arg.FindNextOffset();
        slots = arg.slots;
        SkipNext(endID);
        SetupConnectionsCopy(arg);

        return *this;
    }

    template<class... Args>
    Event<Args...>& Event<Args...>::operator=(Event &&arg) noexcept
    {
        const size_t endID = arg.FindNextOffset();
        slots = std::move(arg.slots);
        SkipNext(endID);
        SetupConnectionsMove(std::move(arg));

        return *this;
    }

    template<class... Args>
    void Event<Args...>::operator()(Args ... args)
    {
        Execute(std::forward<Args>(args)...);
    }

    template<class... Args>
    void Event<Args...>::Execute(Args ... args)
    {
        auto loop = slots.begin();
        next = loop;
        while(loop != slots.end())
        {
            ++next;
            (*loop)(std::forward<Args>(args)...);
            loop = next;
        }
    }

    template<class... Args>
    template<class Ret>
    typename Event<Args...>::Connection Event<Args...>::Subscribe(Ret(*func)(Args...))
    {
        return Subscribe(FunctionT(func));
    }

    template<class... Args>
    template<class Ret, class Obj>
    typename Event<Args...>::Connection Event<Args...>::Subscribe(Ret(Obj::*func)(Args...), Obj& obj)
    {
        return Subscribe(FunctionT(::Chroma::CreateFunction(func, obj)));
    }

    template<class... Args>
    template<class Ret, class Obj>
    typename Event<Args...>::Connection Event<Args...>::Subscribe(Ret(Obj::*func)(Args...) const, const Obj& obj)
    {
        return Subscribe(FunctionT(::Chroma::CreateFunction(func, obj)));
    }

    template<class... Args>
    typename Event<Args...>::Connection Event<Args...>::Subscribe(const FunctionT& function)
    {
        slots.push_front(function);
        connections.push_back(Connection(slots.begin(), *this));
        connections.back().connectionItr = --connections.end();
        return connections.back();
    }

    template<class... Args>
    typename Event<Args...>::Connection Event<Args...>::Subscribe(FunctionT&& function)
    {
        slots.push_front(std::move(function));
        connections.push_back(Connection(slots.begin(), *this));
        connections.back().connectionItr = --connections.end();
        return connections.back();
    }

    template<class... Args>
    void Event<Args...>::Remove(Connection& remove)
    {
        if (slots.empty() || !remove.IsValid() || *remove.owner != this)
            return;

        next = slots.erase(*remove.itr);
        connections.erase(remove.connectionItr);

        *remove.itr = slots.end();
        *remove.owner = nullptr;
    }

    template<class... Args>
    void Event<Args...>::Clear()
    {
        slots.clear();
        connections.clear();
    }

    template<class... Args>
    bool Event<Args...>::IsEmpty() const
    {
        return slots.empty();
    }

    template<class... Args>
    size_t Event<Args...>::FindNextOffset() const
    {
        size_t id = 0;
        for (auto loop = slots.begin(); loop != slots.end(); ++loop)
        {
            if (loop == next)
                return id;
            ++id;
        }

        return id;
    }

    template<class... Args>
    void Event<Args...>::SkipNext(size_t offset)
    {
        size_t id = 0;
        for (auto loop = slots.begin(); loop != slots.end(); ++loop)
        {
            if (id == offset)
            {
                next = loop;
                break;
            }
            ++id;
        }
    }

    template<class... Args>
    void Event<Args...>::SetupConnectionsCopy(const Event &arg)
    {
        connections = arg.connections;
        if (connections.empty())
            return;

        auto connectionLoop = connections.begin();
        for (auto slotLoop = slots.begin(); slotLoop != slots.end(); ++slotLoop)
        {
            connectionLoop->itr.reset(new iterator(slotLoop));
            connectionLoop->owner.reset(new Event*(this));
            connectionLoop->connectionItr = connectionLoop;

            ++connectionLoop;
        }
    }

    template<class... Args>
    void Event<Args...>::SetupConnectionsMove(Event &&arg)
    {
        connections = std::move(arg.connections);
        if (connections.empty())
            return;

        auto connectionLoop = connections.begin();
        for (auto slotLoop = slots.begin(); slotLoop != slots.end(); ++slotLoop)
        {
            *connectionLoop->itr = slotLoop;
            *connectionLoop->owner = this;
            connectionLoop->connectionItr = connectionLoop;

            ++connectionLoop;
        }
    }
}