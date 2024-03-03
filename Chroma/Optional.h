#pragma once

#include <stdexcept>

namespace Chroma
{
    struct OptionalException : public std::runtime_error
    {
        OptionalException();
    };

    template<class T>
    class Optional
    {
    public:
        using ValueT = T;
    public:
        Optional();
        Optional(const ValueT& value);
        Optional(ValueT&& value);
        Optional(const Optional& arg);
        Optional(Optional&& arg);
        template<class... Args>
        explicit Optional(Args&& ... args);
        ~Optional();
        Optional& operator=(const Optional& arg);
        Optional& operator=(Optional&& arg);
        bool operator==(const Optional& arg) const;
        friend bool operator==(const Optional& lhs, const ValueT& rhs)
        {
            return (lhs.valid) ? lhs.value == rhs : false;
        }
        friend bool operator==(const ValueT& lhs, const Optional& rhs)
        {
            return rhs == lhs;
        }
        bool operator!=(const Optional& arg) const;
        friend bool operator!=(const Optional& lhs, const ValueT& rhs)
        {
            return !(lhs == rhs);
        }
        friend bool operator!=(const ValueT& lhs, const Optional& rhs)
        {
            return !(rhs == lhs);
        }
        ValueT* operator->();
        const ValueT* operator->() const;
        ValueT& operator*();
        const ValueT& operator*() const;
        explicit operator ValueT&();
        explicit operator const ValueT&() const;
        explicit operator bool() const;

        // Same thing as Reset()
        void Set();
        void Set(const ValueT& set);
        void Set(ValueT&& set);
        template<class... Args>
        void Set(Args ... args);
        ValueT& Get();
        const ValueT& Get() const;
        // Will return nullptr if not valid, valid ptr if valid
        ValueT* GetAsPtr();
        // Will return nullptr if not valid, valid ptr if valid
        const ValueT* GetAsPtr() const;
        ValueT GetMove();

        void Reset();

        bool IsValid() const;
    private:
        bool valid;

        union
        {
            T value;
        };
    private:
        void ConstructValue(const ValueT& arg);
        void ConstructValue(ValueT&& arg);
        template<class... Args>
        void ConstructValue(Args ... args);
        void DeleteValue();

        ValueT& GetValue();
        const ValueT& GetValue() const;
        ValueT GetValueMove();
    };

    template<class T>
    Optional<T>::Optional() : valid(false)
    {}

    template<class T>
    Optional<T>::Optional(const ValueT& value) : valid(true)
    {
        ConstructValue(value);
    }

    template<class T>
    Optional<T>::Optional(ValueT&& value) : valid(true)
    {
        ConstructValue(std::move(value));
    }

    template<class T>
    Optional<T>::Optional(const Optional& arg) : valid(arg.valid)
    {
        if (valid)
            ConstructValue(arg.value);
    }

    template<class T>
    Optional<T>::Optional(Optional&& arg) : valid(arg.valid)
    {
        if (valid)
        {
            ConstructValue(std::move(arg.value));
            arg.DeleteValue();
        }
    }

    template<class T>
    template<class... Args>
    Optional<T>::Optional(Args&& ... args) : valid(true)
    {
        ConstructValue(std::forward<Args>(args)...);
    }

    template<class T>
    Optional<T>::~Optional()
    {
        DeleteValue();
    }

    template<class T>
    Optional<T>& Optional<T>::operator=(const Optional& arg)
    {
        valid = arg.valid;
        if (valid)
            ConstructValue(arg.value);

        return *this;
    }

    template<class T>
    Optional<T>& Optional<T>::operator=(Optional&& arg)
    {
        valid = arg.valid;
        if (valid)
        {
            ConstructValue(std::move(arg.value));
            arg.DeleteValue();
        }

        return *this;
    }

    template<class T>
    bool Optional<T>::operator==(const Optional& arg) const
    {
        return (valid) ? valid == arg.valid && value == arg.value : valid == arg.valid;
    }

    template<class T>
    bool Optional<T>::operator!=(const Optional& arg) const
    {
        return !(*this == arg);
    }

    template<class T>
    typename Optional<T>::ValueT* Optional<T>::operator->()
    {
        return &GetValue();
    }

    template<class T>
    typename const Optional<T>::ValueT* Optional<T>::operator->() const
    {
        return &GetValue();
    }

    template<class T>
    typename Optional<T>::ValueT& Optional<T>::operator*()
    {
        return GetValue();
    }

    template<class T>
    typename const Optional<T>::ValueT& Optional<T>::operator*() const
    {
        return GetValue();
    }

    template<class T>
    Optional<T>::operator ValueT&()
    {
        return GetValue();
    }

    template<class T>
    Optional<T>::operator const ValueT&() const
    {
        return GetValue();
    }

    template<class T>
    Optional<T>::operator bool() const
    {
        return IsValid();
    }

    template<class T>
    void Optional<T>::Set()
    {
        Reset();
    }

    template<class T>
    void Optional<T>::Set(const ValueT& set)
    {
        DeleteValue();
        valid = true;
        ConstructValue(set);
    }

    template<class T>
    void Optional<T>::Set(ValueT&& set)
    {
        DeleteValue();
        valid = true;
        ConstructValue(std::move(set));
    }

    template<class T>
    template<class... Args>
    void Optional<T>::Set(Args ... args)
    {
        DeleteValue();
        valid = true;
        ConstructValue(std::forward<Args>(args)...);
    }

    template<class T>
    typename Optional<T>::ValueT& Optional<T>::Get()
    {
        return GetValue();
    }

    template<class T>
    typename const Optional<T>::ValueT& Optional<T>::Get() const
    {
        return GetValue();
    }

    template<class T>
    typename Optional<T>::ValueT* Optional<T>::GetAsPtr()
    {
        if (IsValid())
            return &value;

        return nullptr;
    }

    template<class T>
    typename const Optional<T>::ValueT* Optional<T>::GetAsPtr() const
    {
        if (IsValid())
            return value;

        return nullptr;
    }

    template<class T>
    typename Optional<T>::ValueT Optional<T>::GetMove()
    {
        return std::move(GetValueMove());
    }

    template<class T>
    void Optional<T>::Reset()
    {
        DeleteValue();
    }

    template<class T>
    bool Optional<T>::IsValid() const
    {
        return valid;
    }

    template<class T>
    void Optional<T>::ConstructValue(const ValueT& arg)
    {
        new (&value) T(arg);
    }

    template<class T>
    void Optional<T>::ConstructValue(ValueT&& arg)
    {
        new (&value) T(std::move(arg));
    }

    template<class T>
    template<class... Args>
    void Optional<T>::ConstructValue(Args ... args)
    {
        new (&value) T(std::forward<Args>(args)...);
    }

    template<class T>
    void Optional<T>::DeleteValue()
    {
        if (valid)
        {
            value.~T();
            valid = false;
        }
    }

    template<class T>
    typename Optional<T>::ValueT& Optional<T>::GetValue()
    {
        if (!valid)
            throw OptionalException();

        return value;
    }

    template<class T>
    typename const Optional<T>::ValueT& Optional<T>::GetValue() const
    {
        if (!valid)
            throw OptionalException();

        return value;
    }

    template<class T>
    typename Optional<T>::ValueT Optional<T>::GetValueMove()
    {
        if (!valid)
            throw OptionalException();

        T ret(std::move(value));
        DeleteValue();
        return std::move(ret);
    }

}