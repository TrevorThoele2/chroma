#pragma once

#include <vector>
#include <typeindex>
#include <exception>

#include "VariantDetail.h"
#include "VariantException.h"
#include "VariantTraits.h"
#include "Visit.h"

#include "VariadicTemplate.h"

namespace function
{
    template<class... Args>
    bool operator==(const Variant<Args...>& left, const Variant<Args...>& right);
    template<class... Args>
    bool operator!=(const Variant<Args...>& left, const Variant<Args...>& right);
    template<class... Args>
    bool operator<(const Variant<Args...>& left, const Variant<Args...>& right);
    template<class... Args>
    bool operator<=(const Variant<Args...>& left, const Variant<Args...>& right);
    template<class... Args>
    bool operator>(const Variant<Args...>& left, const Variant<Args...>& right);
    template<class... Args>
    bool operator>=(const Variant<Args...>& left, const Variant<Args...>& right);

    template<class... Args>
    class Variant
    {
    public:
        typedef VariadicTemplate<Args...> VariadicTemplateT;
        typedef VariadicTemplateSize ID;
        static constexpr ID count = VariadicTemplateT::count;
        static constexpr ID uninhabitedID = count;

        template<VariadicTemplateSize index>
        using TypeOfParameter = typename VariadicTemplateT::template Parameter<index>::Type;
    private:
        typedef std::type_index Type;
        template<class Variant, class Use>
        friend class detail::VariantSwitch;

        // Require there to be types in this variant
        static_assert(count > 0, "There must be at least one type in this variant.");

        typedef detail::VariantIterationImplementation<Variant<Args...>> IterationImplementation;
        typedef detail::VariantVisitImplementation VisitImplementation;
        friend detail::VariantVisitImplementation;
        typedef detail::VariantVisitStrategy VariantVisitStrategy;
    public:
        Variant();
        template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type = 0>
        Variant(const T& arg);
        template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type = 0>
        Variant(T& arg);
        template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type = 0>
        Variant(T&& arg);
        template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type = 0>
        Variant(T* arg);
        template<class OtherVariantT, typename std::enable_if<VariantTraits<OtherVariantT>::isVariant, int>::type = 0>
        Variant(const OtherVariantT& other);

        Variant(const Variant& arg);
        Variant(Variant&& arg);
        ~Variant();
        Variant& operator=(const Variant& arg);
        Variant& operator=(Variant&& arg);
        friend bool operator== <>(const Variant& left, const Variant& right);
        friend bool operator!= <>(const Variant& left, const Variant& right);
        friend bool operator< <>(const Variant& left, const Variant& right);
        friend bool operator<= <>(const Variant& left, const Variant& right);
        friend bool operator> <>(const Variant& left, const Variant& right);
        friend bool operator>= <>(const Variant& left, const Variant& right);

        void Set();
        template<class T>
        void Set(const T& set);
        template<class T>
        void Set(T& set);
        template<class T>
        void Set(T&& set);
        void SetBytes(const std::vector<char> &set, ID id);
        void SetBytes(std::vector<char> &&set, ID id);

        template<class T>
        T& Get();
        template<class T>
        const T& Get() const;
        template<ID id>
        typename TypeOfParameter<id>& Get();
        template<ID id>
        typename const TypeOfParameter<id>& Get() const;
        // Returns nullptr if the type is incorrect
        template<class T>
        T* GetAsPointer();
        // Returns nullptr if the type is incorrect
        template<class T>
        const T* GetAsPointer() const;
        // Returns nullptr if the type is incorrect
        template<ID id>
        typename TypeOfParameter<id>* GetAsPointer();
        // Returns nullptr if the type is incorrect
        template<ID id>
        typename const TypeOfParameter<id>* GetAsPointer() const;

        std::vector<char> GetAsBytes() const;

        template<class T>
        bool Is() const;
        const Type& GetType() const;
        ID GetTypeAsID() const;
        bool IsInhabited() const;

        template<class... OtherArgs>
        Variant<OtherArgs...> ConvertTo() const;
    private:
        static Type NullType();

        void SetImpl();
        template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type = 0>
        void SetImpl(const T& set);
        template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type = 0>
        void SetImpl(T& set);
        template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type = 0>
        void SetImpl(T&& set);
        template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type = 0>
        void SetImpl(T* set);
        template<class T, typename std::enable_if<VariantTraits<T>::isVariant, int>::type = 0>
        void SetImpl(const T& set);
        template<class T, typename std::enable_if<VariantTraits<T>::isVariant, int>::type = 0>
        void SetImpl(T& set);
        template<class T, typename std::enable_if<VariantTraits<T>::isVariant, int>::type = 0>
        void SetImpl(T&& set);
        template<class T, typename std::enable_if<VariantTraits<T>::isVariant, int>::type = 0>
        void SetImpl(T* set);

        template<class T>
        inline static void CheckForTypeIn()
        {
            static_assert(typename VariadicTemplateT::template IsTypeInside<T>::value, "The type you are using must actually be inside the variant.");
        }

        template<class T>
        inline static void CheckForTypeInConvertible()
        {
            static_assert(typename VariadicTemplateT::template IsConvertibleTypeInside<T>::value, "The type you are using must actually be inside the variant and convertible.");
        }

        template<ID id>
        inline static void CheckForIDIn()
        {
            static_assert(id <= VariadicTemplateT::count, "The ID you are retrieved must actually be inside the variant.");
        }
    private:
        std::vector<char> bytes;
        Type typeIndex;
    };

    template<class... Args>
    Variant<Args...>::Variant() : typeIndex(NullType())
    {}

    template<class... Args>
    template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type>
    Variant<Args...>::Variant(const T& arg) : typeIndex(typeid(T))
    {
        Set(arg);
    }

    template<class... Args>
    template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type>
    Variant<Args...>::Variant(T& arg) : typeIndex(typeid(T))
    {
        Set(arg);
    }

    template<class... Args>
    template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type>
    Variant<Args...>::Variant(T&& arg) : typeIndex(typeid(T))
    {
        Set(std::move(arg));
    }

    template<class... Args>
    template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type>
    Variant<Args...>::Variant(T* arg) : typeIndex(typeid(T*))
    {
        Set(arg);
    }

    template<class... Args>
    template<class OtherVariantT, typename std::enable_if<VariantTraits<OtherVariantT>::isVariant, int>::type>
    Variant<Args...>::Variant(const OtherVariantT& other) : typeIndex(other.GetType())
    {
        if (other.IsInhabited())
            Visit<VisitImplementation>(other, *this, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::CONVERT_TO>{});
        else
            Set();
    }

    template<class... Args>
    Variant<Args...>::Variant(const Variant& arg) : typeIndex(arg.typeIndex)
    {
        if (arg.IsInhabited())
            Visit<VisitImplementation>(arg, *this, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::SET>{});
        else
            Set();
    }

    template<class... Args>
    Variant<Args...>::Variant(Variant&& arg) : typeIndex(std::move(arg.typeIndex))
    {
        if (arg.IsInhabited())
            Visit<VisitImplementation>(std::move(arg), *this, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::TAKE_FROM>{});
        else
            Set();
        arg.Set();
    }

    template<class... Args>
    Variant<Args...>::~Variant()
    {
        if (!IsInhabited())
            return;

        Visit<VisitImplementation>(*this, *this, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::DESTRUCTOR>{});
    }

    template<class... Args>
    Variant<Args...>& Variant<Args...>::operator=(const Variant& arg)
    {
        typeIndex = arg.typeIndex;
        if (arg.IsInhabited())
            Visit<VisitImplementation>(arg, *this, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::SET>{});
        else
            Set();
        return *this;
    }

    template<class... Args>
    Variant<Args...>& Variant<Args...>::operator=(Variant&& arg)
    {
        typeIndex = std::move(arg.typeIndex);
        if (arg.IsInhabited())
            Visit<VisitImplementation>(std::move(arg), *this, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::TAKE_FROM>{});
        else
            Set();
        arg.Set();
        return *this;
    }

    template<class... Args>
    bool operator==(const Variant<Args...>& left, const Variant<Args...>& right)
    {
        typedef Variant<Args...> VariantT;
        if (left.GetType() != right.GetType())
            return false;

        if (!left.IsInhabited())
            return true;

        typedef typename VariantT::VisitImplementation VisitImplementation;
        typedef typename VariantT::VariantVisitStrategy VariantVisitStrategy;
        return VisitReturnMultipleSame<VisitImplementation, bool>(left, right, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::EQUALITY>{});
    }

    template<class... Args>
    bool operator!=(const Variant<Args...>& left, const Variant<Args...>& right)
    {
        typedef Variant<Args...> VariantT;
        if (left.GetType() != right.GetType())
            return false;

        if (!left.IsInhabited())
            return true;

        typedef typename VariantT::VisitImplementation VisitImplementation;
        typedef typename VariantT::VariantVisitStrategy VariantVisitStrategy;
        return VisitReturnMultipleSame<VisitImplementation, bool>(left, right, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::NON_EQUALITY>{});
    }

    template<class... Args>
    bool operator<(const Variant<Args...>& left, const Variant<Args...>& right)
    {
        typedef Variant<Args...> VariantT;
        if (left.GetType() != right.GetType())
            return false;

        if (!left.IsInhabited())
            return true;

        typedef typename VariantT::VisitImplementation VisitImplementation;
        typedef typename VariantT::VariantVisitStrategy VariantVisitStrategy;
        return VisitReturnMultipleSame<VisitImplementation, bool>(left, right, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::LESS>{});
    }

    template<class... Args>
    bool operator<=(const Variant<Args...>& left, const Variant<Args...>& right)
    {
        typedef Variant<Args...> VariantT;
        if (left.GetType() != right.GetType())
            return false;

        if (!left.IsInhabited())
            return true;

        typedef typename VariantT::VisitImplementation VisitImplementation;
        typedef typename VariantT::VariantVisitStrategy VariantVisitStrategy;
        return VisitReturnMultipleSame<VisitImplementation, bool>(left, right, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::LESS_EQUAL>{});
    }

    template<class... Args>
    bool operator>(const Variant<Args...>& left, const Variant<Args...>& right)
    {
        typedef Variant<Args...> VariantT;
        if (left.GetType() != right.GetType())
            return false;

        if (!left.IsInhabited())
            return true;

        typedef typename VariantT::VisitImplementation VisitImplementation;
        typedef typename VariantT::VariantVisitStrategy VariantVisitStrategy;
        return VisitReturnMultipleSame<VisitImplementation, bool>(left, right, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::GREATER>{});
    }

    template<class... Args>
    bool operator>=(const Variant<Args...>& left, const Variant<Args...>& right)
    {
        typedef Variant<Args...> VariantT;
        if (left.GetType() != right.GetType())
            return false;

        if (!left.IsInhabited())
            return true;

        typedef typename VariantT::VisitImplementation VisitImplementation;
        typedef typename VariantT::VariantVisitStrategy VariantVisitStrategy;
        return VisitReturnMultipleSame<VisitImplementation, bool>(left, right, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::GREATER_EQUAL>{});
    }

    template<class... Args>
    void Variant<Args...>::Set()
    {
        SetImpl();
    }

    template<class... Args>
    template<class T>
    void Variant<Args...>::Set(const T& set)
    {
        SetImpl(set);
    }

    template<class... Args>
    template<class T>
    void Variant<Args...>::Set(T& set)
    {
        SetImpl(set);
    }

    template<class... Args>
    template<class T>
    void Variant<Args...>::Set(T&& set)
    {
        SetImpl(std::move(set));
    }

    template<class... Args>
    void Variant<Args...>::SetBytes(const std::vector<char> &set, ID id)
    {
        bytes = set;
        typeIndex = IterationImplementation::GetTypeIndex(id);
    }

    template<class... Args>
    void Variant<Args...>::SetBytes(std::vector<char> &&set, ID id)
    {
        bytes = std::move(set);
        typeIndex = IterationImplementation::GetTypeIndex(id);
    }

    template<class... Args>
    template<class T>
    T& Variant<Args...>::Get()
    {
        CheckForTypeIn<T>();
        if (!IsInhabited())
            throw VariantException("The variant must actually be inhabited.");
        if (GetType() != typeid(T))
            throw VariantException("The variant is not the same type as requested.");
        return reinterpret_cast<T&>(*bytes.data());
    }

    template<class... Args>
    template<class T>
    const T& Variant<Args...>::Get() const
    {
        CheckForTypeIn<T>();
        if(!IsInhabited())
            throw VariantException("The variant must actually be inhabited.");
        if (GetType() != typeid(T))
            throw VariantException("The variant is not the same type as requested.");
        return reinterpret_cast<const T&>(*bytes.data());
    }

    template<class... Args>
    template<VariadicTemplateSize id>
    typename Variant<Args...>::TypeOfParameter<id>& Variant<Args...>::Get()
    {
        typedef TypeOfParameter<id> UseType;
        CheckForIDIn<id>();
        if (!IsInhabited())
            throw VariantException("The variant must actually be inhabited.");
        if (GetType() != typeid(UseType))
            throw VariantException("The variant is not the same type as requested.");
        return reinterpret_cast<UseType&>(*bytes.data());
    }

    template<class... Args>
    template<VariadicTemplateSize id>
    typename const Variant<Args...>::TypeOfParameter<id>& Variant<Args...>::Get() const
    {
        typedef TypeOfParameter<id> UseType;
        CheckForIDIn<id>();
        if (!IsInhabited())
            throw VariantException("The variant must actually be inhabited.");
        if (GetType() != typeid(UseType))
            throw VariantException("The variant is not the same type as requested.");
        return reinterpret_cast<const UseType&>(*bytes.data());
    }

    template<class... Args>
    template<class T>
    T* Variant<Args...>::GetAsPointer()
    {
        CheckForTypeIn<T>();
        if (GetType() != typeid(T))
            return nullptr;
        return &reinterpret_cast<T&>(*bytes.data());
    }

    template<class... Args>
    template<class T>
    const T* Variant<Args...>::GetAsPointer() const
    {
        CheckForTypeIn<T>();
        if (GetType() != typeid(T))
            return nullptr;
        return &reinterpret_cast<const T&>(*bytes.data());
    }

    template<class... Args>
    template<VariadicTemplateSize id>
    typename Variant<Args...>::TypeOfParameter<id>* Variant<Args...>::GetAsPointer()
    {
        typedef TypeOfParameter<id> UseType;
        CheckForIDIn<id>();
        if (GetType() != typeid(UseType))
            return nullptr;
        return &reinterpret_cast<UseType&>(*bytes.data());
    }

    template<class... Args>
    template<VariadicTemplateSize id>
    typename const Variant<Args...>::TypeOfParameter<id>* Variant<Args...>::GetAsPointer() const
    {
        typedef TypeOfParameter<id> UseType;
        CheckForIDIn<id>();
        if (GetType() != typeid(UseType))
            return nullptr;
        return &reinterpret_cast<const UseType&>(*bytes.data());
    }

    template<class... Args>
    std::vector<char> Variant<Args...>::GetAsBytes() const
    {
        return bytes;
    }

    template<class... Args>
    template<class T>
    bool Variant<Args...>::Is() const
    {
        CheckForTypeIn<T>();
        return Type(typeid(T)) == typeIndex;
    }

    template<class... Args>
    typename const Variant<Args...>::Type& Variant<Args...>::GetType() const
    {
        return typeIndex;
    }

    template<class... Args>
    typename Variant<Args...>::ID Variant<Args...>::GetTypeAsID() const
    {
        if (!IsInhabited())
            return uninhabitedID;

        return IterationImplementation::GetID(*this);
    }

    template<class... Args>
    bool Variant<Args...>::IsInhabited() const
    {
        return typeIndex != NullType();
    }

    template<class... Args>
    template<class... OtherArgs>
    Variant<OtherArgs...> Variant<Args...>::ConvertTo() const
    {
        Variant<OtherArgs...> ret;
        Visit<VisitImplementation>(*this, ret, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::CONVERT_TO>{});
        return ret;
    }

    template<class... Args>
    typename Variant<Args...>::Type Variant<Args...>::NullType()
    {
        return typeid(detail::NullT);
    }

    template<class... Args>
    void Variant<Args...>::SetImpl()
    {
        bytes.clear();
        typeIndex = NullType();
    }

    template<class... Args>
    template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type>
    void Variant<Args...>::SetImpl(const T& set)
    {
        CheckForTypeInConvertible<T>();
        bytes.clear();
        bytes.resize(sizeof(T));
        new (reinterpret_cast<T*>(bytes.data())) T(set);
        typeIndex = typeid(T);
    }

    template<class... Args>
    template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type>
    void Variant<Args...>::SetImpl(T& set)
    {
        CheckForTypeInConvertible<T>();
        bytes.clear();
        bytes.resize(sizeof(T));
        new (reinterpret_cast<T*>(bytes.data())) T(set);
        typeIndex = typeid(T);
    }

    template<class... Args>
    template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type>
    void Variant<Args...>::SetImpl(T&& set)
    {
        CheckForTypeInConvertible<T>();
        bytes.clear();
        bytes.resize(sizeof(T));
        new (reinterpret_cast<T*>(bytes.data())) T(std::move(set));
        typeIndex = typeid(T);
    }

    template<class... Args>
    template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type>
    void Variant<Args...>::SetImpl(T* set)
    {
        CheckForTypeInConvertible<T*>();
        bytes.clear();
        bytes.resize(sizeof(T*));
        new (reinterpret_cast<T*>(bytes.data())) T*(set);
        typeIndex = typeid(T*);
    }

    template<class... Args>
    template<class T, typename std::enable_if<VariantTraits<T>::isVariant, int>::type>
    void Variant<Args...>::SetImpl(const T& set)
    {
        Visit<VisitImplementation>(set, *this, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::SET>{});
    }

    template<class... Args>
    template<class T, typename std::enable_if<VariantTraits<T>::isVariant, int>::type>
    void Variant<Args...>::SetImpl(T& set)
    {
        Visit<VisitImplementation>(set, *this, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::SET>{});
    }

    template<class... Args>
    template<class T, typename std::enable_if<VariantTraits<T>::isVariant, int>::type>
    void Variant<Args...>::SetImpl(T&& set)
    {
        Visit<VisitImplementation>(set, *this, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::TAKE_FROM>{});
        set.Set();
    }

    template<class... Args>
    template<class T, typename std::enable_if<VariantTraits<T>::isVariant, int>::type>
    void Variant<Args...>::SetImpl(T* set)
    {
        Visit<VisitImplementation>(set, *this, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::SET>{});
        set.Set();
    }
}