#pragma once

#include <memory>
#include "Type.h"

namespace Chroma
{
    class Any
    {
    public:
        Any() = default;
        template<class RealT>
        explicit Any(const RealT& real) : base(new Derived<RealT>(real))
        {}

        template<class RealT>
        explicit Any(RealT& real) : base(new Derived<RealT>(real))
        {}

        template<class RealT>
        explicit Any(RealT&& real) : base(new Derived<RealT>(std::move(real)))
        {}

        template<class T>
        Any(Type<T> forceType, T real) : base(new Derived<T>(std::move(real)))
        {}

        Any(const Any& arg) : base((arg.base) ? arg.base->Clone() : nullptr)
        {}

        Any(Any&& arg) : base(std::move(arg.base))
        {}

        Any& operator=(const Any& arg)
        {
            (arg.IsOccupied()) ? base.reset(arg.base->Clone()) : base.reset();
            return *this;
        }

        Any& operator=(Any&& arg)
        {
            base = std::move(arg.base);
            return *this;
        }

        bool operator==(const Any& arg) const
        {
            return base == arg.base;
        }

        bool operator!=(const Any& arg) const
        {
            return !(*this == arg);
        }

        void Set()
        {
            Reset();
        }

        template<class RealT>
        void Set(const RealT& real)
        {
            base.reset(new Derived<RealT>(real));
        }

        template<class RealT>
        void Set(RealT& real)
        {
            base.reset(new Derived<RealT>(real));
        }

        template<class RealT>
        void Set(RealT&& real)
        {
            base.reset(new Derived<RealT>(std::move(real)));
        }

        void Reset()
        {
            base.reset();
        }

        bool IsOccupied() const
        {
            return base.get() != nullptr;
        }

        // This does not check if the type is the same as what's stored here
        template<class RealT>
        RealT& As()
        {
            return static_cast<Derived<RealT>*>(base.get())->real;
        }

        // This does not check if the type is the same as what's stored here
        template<class RealT>
        const RealT& As() const
        {
            return static_cast<Derived<RealT>*>(base.get())->real;
        }
    private:
        class Base
        {
        public:
            virtual ~Base() = 0 {}
            virtual Base* Clone() const = 0;
        };

        template<class RealT>
        class Derived : public Base
        {
        public:
            RealT real;
            Derived(const RealT& real) : real(real)
            {}

            Derived(RealT& real) : real(real)
            {}

            Derived(RealT&& real) : real(std::move(real))
            {}

            Derived* Clone() const override
            {
                return new Derived(*this);
            }
        };
    private:
        std::unique_ptr<Base> base;
    };
}