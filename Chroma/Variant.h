#pragma once

#include <vector>
#include <typeindex>

#include "VariantDetail.h"
#include "VariantException.h"
#include "VariantTraits.h"
#include "Visit.h"

#include "VariadicTemplate.h"
#include "SelectableType.h"

namespace Chroma
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
    private:
        using SelectedType = SelectableType<Args...>;
    public:
        using VariadicTemplateT = VariadicTemplate<Args...>;
        using ID = VariadicTemplateSize;
        static constexpr ID count = VariadicTemplateT::count;
        static constexpr ID uninhabitedID = count;

        template<VariadicTemplateSize index>
        using TypeOfParameter = typename VariadicTemplateT::template Parameter<index>::Type;

        using TypeIndex = typename SelectedType::TypeIndex;
    private:
        template<class Variant, class Use>
        friend class detail::VariantSwitch;

        using IterationImplementation = detail::VariantIterationImplementation<Variant<Args...>>;
        friend detail::VariantIterationImplementation<Variant<Args...>>;
        using VisitImplementation = detail::VariantVisitImplementation;
        friend detail::VariantVisitImplementation;
        using VariantVisitStrategy = detail::VariantVisitStrategy;
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
        const TypeIndex& GetType() const;
        ID GetTypeAsID() const;
        bool IsInhabited() const;

        template<class... OtherArgs>
        Variant<OtherArgs...> ConvertTo() const;
    private:
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
            static_assert(
                VariadicTemplateT::template IsTypeInside<T>::value,
                "The type you are using must actually be inside the variant.");
        }

        template<class T>
        inline static void CheckForTypeInConvertible()
        {
            static_assert(
                VariadicTemplateT::template IsConvertibleTypeInside<T>::value,
                "The type you are using must actually be inside the variant and convertible.");
        }

        template<ID id>
        inline static void CheckForIDIn()
        {
            static_assert(
                id <= VariadicTemplateT::count,
                "The ID you are retrieved must actually be inside the variant.");
        }
    private:
        std::vector<char> bytes;
    private:
        SelectedType selectedType;
    };

    template<class... Args>
    Variant<Args...>::Variant()
    {}

    template<class... Args>
    template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type>
    Variant<Args...>::Variant(const T& arg) : selectedType(Type<T>{})
    {
        Set(arg);
    }

    template<class... Args>
    template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type>
    Variant<Args...>::Variant(T& arg) : selectedType(Type<T>{})
    {
        Set(arg);
    }

    template<class... Args>
    template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type>
    Variant<Args...>::Variant(T&& arg) : selectedType(Type<T>{})
    {
        Set(std::move(arg));
    }

    template<class... Args>
    template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type>
    Variant<Args...>::Variant(T* arg) : selectedType(Type<T>{})
    {
        Set(arg);
    }

    template<class... Args>
    template<class OtherVariantT, typename std::enable_if<VariantTraits<OtherVariantT>::isVariant, int>::type>
    Variant<Args...>::Variant(const OtherVariantT& other)
    {
        if (other.IsInhabited())
            Visit<VisitImplementation>(other, *this, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::CONVERT_TO>{});
        else
            Set();
    }

    template<class... Args>
    Variant<Args...>::Variant(const Variant& arg) : selectedType(arg.selectedType)
    {
        if (arg.IsInhabited())
            Visit<VisitImplementation>(arg, *this, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::SET>{});
        else
            Set();
    }

    template<class... Args>
    Variant<Args...>::Variant(Variant&& arg) : selectedType(std::move(arg.selectedType))
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
        selectedType = arg.selectedType;
        if (arg.IsInhabited())
            Visit<VisitImplementation>(arg, *this, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::SET>{});
        else
            Set();
        return *this;
    }

    template<class... Args>
    Variant<Args...>& Variant<Args...>::operator=(Variant&& arg)
    {
        selectedType = std::move(arg.selectedType);
        if (arg.IsInhabited())
            Visit<VisitImplementation>(arg, *this, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::TAKE_FROM>{});
        else
            Set();
        arg.Set();
        return *this;
    }

    template<class... Args>
    bool operator==(const Variant<Args...>& left, const Variant<Args...>& right)
    {
        using VariantT = Variant<Args...>;
        if (left.GetType() != right.GetType())
            return false;

        if (!left.IsInhabited())
            return true;

        using VisitImplementation = typename VariantT::VisitImplementation;
        using VariantVisitStrategy = typename VariantT::VariantVisitStrategy;
        return VisitReturnMultipleSame<VisitImplementation, bool>(
            left, right, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::EQUALITY>{});
    }

    template<class... Args>
    bool operator!=(const Variant<Args...>& left, const Variant<Args...>& right)
    {
        using VariantT = Variant<Args...>;
        if (left.GetType() != right.GetType())
            return false;

        if (!left.IsInhabited())
            return true;

        using VisitImplementation = typename VariantT::VisitImplementation;
        using VariantVisitStrategy = typename VariantT::VariantVisitStrategy;
        return VisitReturnMultipleSame<VisitImplementation, bool>(
            left, right, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::NON_EQUALITY>{});
    }

    template<class... Args>
    bool operator<(const Variant<Args...>& left, const Variant<Args...>& right)
    {
        using VariantT = Variant<Args...>;
        if (left.GetType() != right.GetType())
            return false;

        if (!left.IsInhabited())
            return true;

        using VisitImplementation = typename VariantT::VisitImplementation;
        using VariantVisitStrategy = typename VariantT::VariantVisitStrategy;
        return VisitReturnMultipleSame<VisitImplementation, bool>(
            left, right, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::LESS>{});
    }

    template<class... Args>
    bool operator<=(const Variant<Args...>& left, const Variant<Args...>& right)
    {
        using VariantT = Variant<Args...>;
        if (left.GetType() != right.GetType())
            return false;

        if (!left.IsInhabited())
            return true;

        using VisitImplementation = typename VariantT::VisitImplementation;
        using VariantVisitStrategy = typename VariantT::VariantVisitStrategy;
        return VisitReturnMultipleSame<VisitImplementation, bool>(
            left, right, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::LESS_EQUAL>{});
    }

    template<class... Args>
    bool operator>(const Variant<Args...>& left, const Variant<Args...>& right)
    {
        using VariantT = Variant<Args...>;
        if (left.GetType() != right.GetType())
            return false;

        if (!left.IsInhabited())
            return true;

        using VisitImplementation = typename VariantT::VisitImplementation;
        using VariantVisitStrategy = typename VariantT::VariantVisitStrategy;
        return VisitReturnMultipleSame<VisitImplementation, bool>(
            left, right, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::GREATER>{});
    }

    template<class... Args>
    bool operator>=(const Variant<Args...>& left, const Variant<Args...>& right)
    {
        using VariantT = Variant<Args...>;
        if (left.GetType() != right.GetType())
            return false;

        if (!left.IsInhabited())
            return true;

        using VisitImplementation = typename VariantT::VisitImplementation;
        using VariantVisitStrategy = typename VariantT::VariantVisitStrategy;
        return VisitReturnMultipleSame<VisitImplementation, bool>(
            left, right, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::GREATER_EQUAL>{});
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
        IterationImplementation::Select(id, selectedType);
    }

    template<class... Args>
    void Variant<Args...>::SetBytes(std::vector<char> &&set, ID id)
    {
        bytes = std::move(set);
        IterationImplementation::Select(id, selectedType);
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
    typename Variant<Args...>::template TypeOfParameter<id>& Variant<Args...>::Get()
    {
        using UseType = TypeOfParameter<id>;
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
        using UseType = TypeOfParameter<id>;
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
    typename Variant<Args...>::template TypeOfParameter<id>* Variant<Args...>::GetAsPointer()
    {
        using UseType = TypeOfParameter<id>;
        CheckForIDIn<id>();
        if (GetType() != typeid(UseType))
            return nullptr;
        return &reinterpret_cast<UseType&>(*bytes.data());
    }

    template<class... Args>
    template<VariadicTemplateSize id>
    typename const Variant<Args...>::TypeOfParameter<id>* Variant<Args...>::GetAsPointer() const
    {
        using UseType = TypeOfParameter<id>;
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
        return selectedType.Is<T>();
    }

    template<class... Args>
    typename const Variant<Args...>::TypeIndex& Variant<Args...>::GetType() const
    {
        return selectedType.Selected();
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
        return selectedType.HasSelected();
    }

    template<class... Args>
    template<class... OtherArgs>
    Variant<OtherArgs...> Variant<Args...>::ConvertTo() const
    {
        Variant<OtherArgs...> ret;
        Visit<VisitImplementation>(
            *this, ret, StrategySelector<VariantVisitStrategy, VariantVisitStrategy::CONVERT_TO>{});
        return ret;
    }

    template<class... Args>
    void Variant<Args...>::SetImpl()
    {
        bytes.clear();
        selectedType.Unselect();
    }

    template<class... Args>
    template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type>
    void Variant<Args...>::SetImpl(const T& set)
    {
        CheckForTypeInConvertible<T>();
        bytes.clear();
        bytes.resize(sizeof(T));
        new (reinterpret_cast<T*>(bytes.data())) T(set);
        selectedType.Select<T>();
    }

    template<class... Args>
    template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type>
    void Variant<Args...>::SetImpl(T& set)
    {
        CheckForTypeInConvertible<T>();
        bytes.clear();
        bytes.resize(sizeof(T));
        new (reinterpret_cast<T*>(bytes.data())) T(set);
        selectedType.Select<T>();
    }

    template<class... Args>
    template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type>
    void Variant<Args...>::SetImpl(T&& set)
    {
        CheckForTypeInConvertible<T>();
        bytes.clear();
        bytes.resize(sizeof(T));
        new (reinterpret_cast<T*>(bytes.data())) T(std::move(set));
        selectedType.Select<T>();
    }

    template<class... Args>
    template<class T, typename std::enable_if<!VariantTraits<T>::isVariant, int>::type>
    void Variant<Args...>::SetImpl(T* set)
    {
        CheckForTypeInConvertible<T*>();
        bytes.clear();
        bytes.resize(sizeof(T*));
        new (reinterpret_cast<T*>(bytes.data())) T*(set);
        selectedType.Select<T>();
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