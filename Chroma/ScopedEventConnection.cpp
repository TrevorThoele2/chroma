#include "ScopedEventConnection.h"

namespace Chroma
{
    ScopedEventConnection::ScopedEventConnection(const ScopedEventConnection& arg) : base((arg.base) ? arg.base->Clone() : nullptr)
    {}

    ScopedEventConnection::ScopedEventConnection(ScopedEventConnection&& arg) : base(std::move(arg.base))
    {}

    ScopedEventConnection::~ScopedEventConnection()
    {
        if (base)
            base->Sever();
    }

    ScopedEventConnection& ScopedEventConnection::operator=(const ScopedEventConnection& arg)
    {
        base.reset((arg.base) ? arg.base->Clone() : nullptr);
        return *this;
    }

    ScopedEventConnection& ScopedEventConnection::operator=(ScopedEventConnection&& arg)
    {
        base = std::move(arg.base);
        return *this;
    }

    bool ScopedEventConnection::operator==(const ScopedEventConnection& arg) const
    {
        return base == arg.base;
    }

    bool ScopedEventConnection::operator!=(const ScopedEventConnection& arg) const
    {
        return !(*this == arg);
    }

    void ScopedEventConnection::Reset()
    {
        base.reset();
    }

    void ScopedEventConnection::Sever()
    {
        if (base)
        {
            base->Sever();
            base.reset();
        }
    }

    bool ScopedEventConnection::IsValid() const
    {
        return base.get() != nullptr && base->IsValid();
    }

    ScopedEventConnection::Base::~Base()
    {}
}