#pragma once

#include <memory>

#include "EventObject.h"

namespace Chroma
{
    template<class... Args>
    class EventConnection
    {
    public:
        using EventT = Event<Args...>;
    public:
        EventConnection() = default;
        EventConnection(const EventConnection& arg);
        EventConnection(EventConnection&& arg) noexcept;
        EventConnection& operator=(const EventConnection& arg);
        EventConnection& operator=(EventConnection&& arg) noexcept;
        bool operator==(const EventConnection& arg) const;
        bool operator!=(const EventConnection& arg) const;

        void Execute(Args && ... args);
        void Sever();
        bool IsValid() const;
    private:
        using iterator = typename EventT::iterator;

        std::shared_ptr<iterator> itr;
        std::shared_ptr<EventT*> owner;

        typename EventT::Connections::iterator connectionItr;

        EventConnection(iterator itr, EventT& owner);
    private:
        friend EventT;
    };

    template<class... Args>
    EventConnection<Args...>::EventConnection(const EventConnection& arg) : itr(arg.itr), owner(arg.owner), connectionItr(arg.connectionItr)
    {}

    template<class... Args>
    EventConnection<Args...>::EventConnection(EventConnection&& arg) noexcept : itr(std::move(arg.itr)), owner(std::move(arg.owner)), connectionItr(std::move(arg.connectionItr))
    {}

    template<class... Args>
    EventConnection<Args...>& EventConnection<Args...>::operator=(const EventConnection& arg)
    {
        itr = arg.itr;
        owner = arg.owner;
        connectionItr = arg.connectionItr;
        return *this;
    }

    template<class... Args>
    EventConnection<Args...>& EventConnection<Args...>::operator=(EventConnection&& arg) noexcept
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
}