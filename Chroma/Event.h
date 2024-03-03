#pragma once

#include <vector>
#include <list>
#include <memory>
#include "Function.h"

namespace Chroma
{
    template<class... Args>
    class EventConnection;
    template<class... Args>
    class Event
    {
    public:
        typedef Function<void, Args...> FunctionT;
        typedef EventConnection<Args...> Connection;
    public:
        Event();
        Event(const Event &arg);
        Event(Event &&arg);
        Event& operator=(const Event &arg);
        Event& operator=(Event &&arg);

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
        bool IsEmpty() const;
    private:
        friend class Connection;
    private:
        typedef std::list<FunctionT> Slots;
        typedef typename Slots::iterator iterator;
        typedef typename Slots::const_iterator const_iterator;

        Slots slots;
        // Used when executing this event
        iterator next;

        typedef std::list<Connection> Connections;
        Connections connections;

        // Finds the offset of the next iterator
        size_t FindNextOffset() const;
        // Sets the next iterator by an offset from the beginning of the list
        void SkipNext(size_t offset);

        void SetupConnectionsMove(Event &&arg);
        void SetupConnectionsCopy(const Event &arg);
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
    Event<Args...>::Event(Event &&arg) : slots(std::move(arg.slots))
    {
        next = slots.end();
        SetupConnectionsMove(std::move(arg));
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
    Event<Args...>& Event<Args...>::operator=(Event &&arg)
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
            loop->Execute(std::forward<Args>(args)...);
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
        return Subscribe(FunctionT(func, &obj));
    }

    template<class... Args>
    template<class Ret, class Obj>
    typename Event<Args...>::Connection Event<Args...>::Subscribe(Ret(Obj::*func)(Args...) const, const Obj& obj)
    {
        return Subscribe(FunctionT(func, &obj));
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
        auto connectionLoop = connections.begin();
        for (auto slotLoop = slots.begin(); slotLoop != slots.end(); ++slotLoop)
        {
            *connectionLoop->itr = slotLoop;
            *connectionLoop->owner = this;
            connectionLoop->connectionItr = connectionLoop;

            ++connectionLoop;
        }
    }

    template<class... Args>
    class EventConnection
    {
    public:
        typedef Event<Args...> EventT;
    public:
        EventConnection() = default;
        EventConnection(const EventConnection& arg);
        EventConnection(EventConnection&& arg);
        EventConnection& operator=(const EventConnection& arg);
        EventConnection& operator=(EventConnection&& arg);
        bool operator==(const EventConnection& arg) const;
        bool operator!=(const EventConnection& arg) const;

        void Execute(Args && ... args);
        void Sever();
        bool IsValid() const;
    private:
        typedef typename EventT::iterator iterator;

        std::shared_ptr<iterator> itr;
        std::shared_ptr<EventT*> owner;

        typename EventT::Connections::iterator connectionItr;

        EventConnection(iterator itr, EventT& owner);

        friend EventT;
    };

    template<class... Args>
    EventConnection<Args...>::EventConnection(const EventConnection& arg) : itr(arg.itr), owner(arg.owner), connectionItr(arg.connectionItr)
    {}

    template<class... Args>
    EventConnection<Args...>::EventConnection(EventConnection&& arg) : itr(std::move(arg.itr)), owner(std::move(arg.owner)), connectionItr(std::move(arg.connectionItr))
    {}

    template<class... Args>
    typename EventConnection<Args...>& EventConnection<Args...>::operator=(const EventConnection& arg)
    {
        itr = arg.itr;
        owner = arg.owner;
        connectionItr = arg.connectionItr;
        return *this;
    }

    template<class... Args>
    typename EventConnection<Args...>& EventConnection<Args...>::operator=(EventConnection&& arg)
    {
        itr = std::move(arg.itr);
        arg.itr.reset(new iterator((*arg.owner)->slots.end()));

        owner = std::move(arg.owner);
        arg.owner.reset(new Event<Args...>*(nullptr));

        connectionItr = std::move(arg.connectionItr);

        return *this;
    }

    template<class... Args>
    bool EventConnection<Args...>::operator==(const EventConnection& arg) const
    {
        return *itr == *arg.itr && ((owner && arg.owner) ? *owner == *arg.owner : owner == arg.owner);
    }

    template<class... Args>
    bool EventConnection<Args...>::operator!=(const EventConnection& arg) const
    {
        return !(*this == arg);
    }

    template<class... Args>
    void EventConnection<Args...>::Execute(Args && ... args)
    {
        if (!IsValid())
            return;

        (*itr)->Execute(std::forward<Args>(args)...);
    }

    template<class... Args>
    void EventConnection<Args...>::Sever()
    {
        if (!IsValid())
            return;

        (*owner)->Remove(*this);
    }

    template<class... Args>
    bool EventConnection<Args...>::IsValid() const
    {
        return owner && *owner && (*itr) != (*owner)->slots.end();
    }

    template<class... Args>
    EventConnection<Args...>::EventConnection(iterator itr, EventT& owner) : itr(new iterator(itr)), owner(new EventT*(&owner))
    {}

    using NullEvent = Event<>;
}