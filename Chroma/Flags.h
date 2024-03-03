#pragma once

#include <type_traits>
#include <initializer_list>

namespace Chroma
{
    // Flags given should be 1, 2, 3, 4, etc.
    // These represent the bit position
    template<class EnumT>
    class Flags
    {
    public:
        typedef typename std::underlying_type<EnumT>::type Bits;
    public:
        Flags();
        Flags(EnumT flag);
        Flags(const std::initializer_list<EnumT>& list);
        explicit Flags(Bits bits);

        Flags<EnumT>& operator=(const Flags<EnumT>& arg);
        Flags<EnumT>& operator=(EnumT arg);
        Flags<EnumT>& operator|(const Flags<EnumT>& arg);
        Flags<EnumT>& operator|(EnumT arg);
        Flags<EnumT>& operator|=(const Flags<EnumT>& arg);
        Flags<EnumT>& operator|=(EnumT arg);

        bool operator==(const Flags<EnumT>& arg) const;
        bool operator!=(const Flags<EnumT>& arg) const;

        void Set(EnumT flag, bool set = true);
        void Reset();
        void Flip(EnumT flag);
        bool Get(EnumT flag) const;
        bool Has(EnumT flag) const;
        // Checks if all of the bits are the given value
        bool CheckAll(bool value = true);
        Bits GetBits() const;
    private:
        Bits bits;
    };

    template<class EnumT>
    Flags<EnumT>::Flags() : bits(0)
    {}

    template<class EnumT>
    Flags<EnumT>::Flags(EnumT flag) : bits(0)
    {
        Set(flag);
    }

    template<class EnumT>
    Flags<EnumT>::Flags(const std::initializer_list<EnumT>& list) : bits(0)
    {
        for (auto& loop : list)
            Set(loop);
    }

    template<class EnumT>
    Flags<EnumT>::Flags(Bits bits) : bits(bits)
    {}

    template<class EnumT>
    Flags<EnumT>& Flags<EnumT>::operator=(const Flags<EnumT>& arg)
    {
        bits = arg.bits;

        return *this;
    }

    template<class EnumT>
    Flags<EnumT>& Flags<EnumT>::operator=(EnumT arg)
    {
        Reset();
        bits = static_cast<Bits>(arg);
        return *this;
    }

    template<class EnumT>
    Flags<EnumT>& Flags<EnumT>::operator|(const Flags<EnumT>& arg)
    {
        bits.set(arg);
    }

    template<class EnumT>
    Flags<EnumT>& Flags<EnumT>::operator|(EnumT arg)
    {
        Set(arg);
        return *this;
    }

    template<class EnumT>
    Flags<EnumT>& Flags<EnumT>::operator|=(const Flags<EnumT>& arg)
    {
        bits.set(arg.bits);

        return *this;
    }

    template<class EnumT>
    Flags<EnumT>& Flags<EnumT>::operator|=(EnumT arg)
    {
        Set(arg);
    }

    template<class EnumT>
    bool Flags<EnumT>::operator==(const Flags<EnumT>& arg) const
    {
        return bits == arg.bits;
    }

    template<class EnumT>
    bool Flags<EnumT>::operator!=(const Flags<EnumT>& arg) const
    {
        return !(*this == arg);
    }

    template<class EnumT>
    void Flags<EnumT>::Set(EnumT flag, bool setTo)
    {
        if (Get(flag) != setTo)
            Flip(flag);
    }

    template<class EnumT>
    void Flags<EnumT>::Reset()
    {
        bits = 0;
    }

    template<class EnumT>
    void Flags<EnumT>::Flip(EnumT flag)
    {
        bits ^= 1 << (static_cast<Bits>(flag) - 1);
    }

    template<class EnumT>
    bool Flags<EnumT>::Get(EnumT flag) const
    {
        return ((bits >> (static_cast<Bits>(flag) - 1)) & 1) != 0;
    }

    template<class EnumT>
    bool Flags<EnumT>::Has(EnumT flag) const
    {
        return Get(flag);
    }

    template<class EnumT>
    bool Flags<EnumT>::CheckAll(bool value)
    {
        if (value)
            return bits == std::numeric_limits<Bits>::max();
        else
            return bits == 0;
    }

    template<class EnumT>
    typename Flags<EnumT>::Bits Flags<EnumT>::GetBits() const
    {
        return bits;
    }
}