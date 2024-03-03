#pragma once

#include "EventConnection.h"

#include <assert.h>

namespace Chroma
{
    class ScopedEventConnection
    {
    public:
        ScopedEventConnection() = default;
        template<class... Args>
        ScopedEventConnection(const EventConnection<Args...>& wrapped);
        template<class... Args>
        ScopedEventConnection(EventConnection<Args...>&& wrapped);
        ScopedEventConnection(const ScopedEventConnection& arg);
        ScopedEventConnection(ScopedEventConnection&& arg) noexcept;
        ~ScopedEventConnection();

        ScopedEventConnection& operator=(const ScopedEventConnection& arg);
        ScopedEventConnection& operator=(ScopedEventConnection&& arg) noexcept;

        bool operator==(const ScopedEventConnection& arg) const;
        bool operator!=(const ScopedEventConnection& arg) const;

        template<class... Args>
        void Set(const EventConnection<Args...>& set);
        template<class... Args>
        void Set(EventConnection<Args...>&& set);
        // Does not remove this slot from the event
        void Reset();

        template<class... Args>
        void Execute(Args&& ... args);
        // Removes slot from event
        void Sever();

        [[nodiscard]] bool IsValid() const;
    private:
        class Base
        {
        public:
            virtual ~Base() = 0;

            [[nodiscard]] virtual Base* Clone() const = 0;

            virtual void Sever() = 0;
            [[nodiscard]] virtual bool IsValid() const = 0;
        };

        template<class... Args>
        class Derived final : public Base
        {
        public:
            using Wrapped = typename Event<Args...>::Connection;
            Wrapped wrapped;
        public:
            Derived(const Wrapped &wrapped);
            Derived(Wrapped &&arg);
            ~Derived();

            Derived* Clone() const override;

            void Sever() override;
            [[nodiscard]] bool IsValid() const override;
        };
    private:
        std::unique_ptr<Base> base;
    };

    template<class... Args>
    ScopedEventConnection::Derived<Args...>::Derived(const Wrapped& wrapped) : wrapped(wrapped)
    {}

    template<class... Args>
    ScopedEventConnection::Derived<Args...>::Derived(Wrapped&& wrapped) : wrapped(std::move(wrapped))
    {}

    template<class... Args>
    ScopedEventConnection::Derived<Args...>::~Derived()
    {
        Sever();
    }

    template<class... Args>
    ScopedEventConnection::Derived<Args...>* ScopedEventConnection::Derived<Args...>::Clone() const
    {
        return new Derived(*this);
    }

    template<class... Args>
    void ScopedEventConnection::Derived<Args...>::Sever()
    {
        if (IsValid())
            wrapped.Sever();
    }

    template<class... Args>
    bool ScopedEventConnection::Derived<Args...>::IsValid() const
    {
        return wrapped.IsValid();
    }

    template<class... Args>
    ScopedEventConnection::ScopedEventConnection(const EventConnection<Args...>& wrapped) : base(new Derived<Args...>(wrapped))
    {}

    template<class... Args>
    ScopedEventConnection::ScopedEventConnection(EventConnection<Args...>&& wrapped) : base(new Derived<Args...>(std::move(wrapped)))
    {}

    template<class... Args>
    void ScopedEventConnection::Set(const EventConnection<Args...>& set)
    {
        base.reset(new Derived<Args...>(set));
    }

    template<class... Args>
    void ScopedEventConnection::Set(EventConnection<Args...>&& set)
    {
        base.reset(new Derived<Args...>(std::move(set)));
    }

    template<class... Args>
    void ScopedEventConnection::Execute(Args&& ... args)
    {
        auto converted = dynamic_cast<Derived<Args...>*>(base.get());
        assert(converted != nullptr);
        converted->wrapped.Execute(std::forward<Args>(args)...);
    }
}