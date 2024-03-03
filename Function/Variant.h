#pragma once

#include <typeindex>
#include <exception>
#include "VariadicTemplate.h"

namespace function
{
    class VariantException : public std::exception
    {
    public:
        VariantException(const char* message) : exception(message)
        {}
    };

    template<class EnumT, EnumT selector>
    struct StrategySelector {};

    namespace detail
    {
        template<class Variant, class Use>
        class VariantSwitch;
    }

    template<class... Args>
    class Variant
    {
    public:
        typedef VariadicTemplate<Args...> VariadicTemplateT;
        typedef size_t ID;
    private:
        template<class Variant, class Use>
        friend class detail::VariantSwitch;

        template<class T>
        class IsTypeIn
        {
        private:
            template<::function::VariadicTemplateSize index, class Type>
            struct TypeStep
            {
                typedef typename VariadicTemplateT::template Parameter<index - 1>::Type PieceType;
                static constexpr bool areTypesSame = std::is_same<Type, PieceType>::value;
                // Check if types are the same - if they are, just return true, otherwise check lower down
                static constexpr bool result = (areTypesSame) ? true : TypeStep<index - 1, Type>::result;
            };

            template<class Type>
            struct TypeStep<0, Type>
            {
                typedef typename VariadicTemplateT::template Parameter<0>::Type PieceType;
                static constexpr bool areTypesSame = std::is_same<Type, PieceType>::value;
                static constexpr bool result = (areTypesSame) ? true : false;
            };
        public:
            static constexpr bool result = TypeStep<VariadicTemplateT::count, T>::result;
        };

        class IterationImplementation
        {
        private:
            template<class BiggestType, class CheckType, bool val>
            struct SubGetMasterType;

            template<class BiggestType, class CheckType>
            struct SubGetMasterType<BiggestType, CheckType, true>
            {
                typedef CheckType T;
            };

            template<class BiggestType, class CheckType>
            struct SubGetMasterType<BiggestType, CheckType, false>
            {
                typedef BiggestType T;
            };

            template<::function::VariadicTemplateSize index, class BiggestType>
            struct GetMasterType
            {
                typedef typename VariadicTemplateT::Parameter<index - 1>::Type PieceType;
                // Check if this piece type is bigger than the biggest type yet
                typedef typename SubGetMasterType<BiggestType, PieceType, (sizeof(PieceType) > sizeof(BiggestType))>::T NewBiggest;
                typedef typename GetMasterType<index - 1, NewBiggest>::T T;
            };

            template<class BiggestType>
            struct GetMasterType<0, BiggestType>
            {
                typedef typename VariadicTemplateT::Parameter<0> PieceType;
                // Check if this piece type is bigger than the biggest type yet
                typedef typename SubGetMasterType<BiggestType, PieceType, (sizeof(PieceType) > sizeof(BiggestType))>::T NewBiggest;
                typedef NewBiggest T;
            };
        public:
            typedef typename GetMasterType<VariadicTemplateT::count, typename VariadicTemplateT::Parameter<VariadicTemplateT::count - 1>::Type>::T MasterType;
        private:
            template<::function::VariadicTemplateSize index, ::function::VariadicTemplateSize checkIndex>
            struct SubDuplicationStep
            {
                typedef typename VariadicTemplateT::template Parameter<index - 1>::Type PieceType;
                typedef typename VariadicTemplateT::template Parameter<checkIndex>::Type CheckType;
                static constexpr bool areIndexesSame = index - 1 == checkIndex;
                // If the indexes are the same, then pass this check up
                static constexpr bool result = (areIndexesSame) ? SubDuplicationStep<index - 1, checkIndex>::result :
                    // Otherwise check for real
                    ((!std::is_same<CheckType, PieceType>::value) ? SubDuplicationStep<index - 1, checkIndex>::result : true);
            };

            template<::function::VariadicTemplateSize checkIndex>
            struct SubDuplicationStep<0, checkIndex>
            {
                typedef typename VariadicTemplateT::template Parameter<0>::Type PieceType;
                typedef typename VariadicTemplateT::template Parameter<checkIndex>::Type CheckType;
                static constexpr bool areIndexesSame = 0 == checkIndex;
                // If the indexes are the same, then return false
                static constexpr bool result = (areIndexesSame) ? false :
                    // Otherwise check for real
                    (std::is_same<CheckType, PieceType>::value);
            };

            template<::function::VariadicTemplateSize index>
            struct DuplicationStep
            {
                // If the sub impl returns true (it found two with the same types), then return false (so the static assert fails)
                static constexpr bool result = (SubDuplicationStep<VariadicTemplateT::count, index - 1>::result) ? false : DuplicationStep<index - 1>::result;
            };

            template<>
            struct DuplicationStep<0>
            {
                // If the sub impl returns true (it found two with the same types), then return false (so the static assert fails)
                static constexpr bool result = (SubDuplicationStep<VariadicTemplateT::count, 0>::result) ? false : true;
            };
        public:
            static constexpr bool duplicationResult = DuplicationStep<VariadicTemplateT::count>::result;
        private:
            template<::function::VariadicTemplateSize index>
            struct IDStep
            {
                typedef typename VariadicTemplateT::template Parameter<index - 1>::Type PieceType;

                static ID GetID(const Variant& var)
                {
                    if (std::type_index(typeid(PieceType)) == var.GetType())
                        return index - 1;
                    else
                        return IDStep<index - 1>::GetID(var);
                }

                static std::type_index GetTypeIndex(ID id)
                {
                    if (id == index - 1)
                        return std::type_index(typeid(PieceType));
                    else
                        return IDStep<index - 1>::GetTypeIndex(id);
                }
            };

            template<>
            struct IDStep<0>
            {
                typedef typename VariadicTemplateT::template Parameter<0>::Type PieceType;

                static ID GetID(const Variant& var)
                {
                     return 0;
                }

                static std::type_index GetTypeIndex(ID id)
                {
                    return std::type_index(typeid(PieceType));
                }
            };
        public:
            static ID GetID(const Variant& var)
            {
                return IDStep<VariadicTemplateT::count>::GetID(var);
            }

            static std::type_index GetTypeIndex(ID id)
            {
                return IDStep<VariadicTemplateT::count>::GetTypeIndex(id);
            }
        };

        // Require there to be types in this variant
        static_assert(VariadicTemplateT::count > 0, "There must be at least one type in this variant.");
        // Allow no duplicates
        static_assert(IterationImplementation::duplicationResult, "There must be no duplicate types inside of a variant.");
    public:
        typedef typename IterationImplementation::MasterType MasterType;
    public:
        Variant();
        template<class T, typename std::enable_if<IsTypeIn<T>::result, int>::type = 0>
        Variant(const T& arg) : typeIndex(typeid(T))
        {
            Set(arg);
        }

        template<class T, typename std::enable_if<IsTypeIn<T>::result, int>::type = 0>
        Variant(T&& arg) : typeIndex(typeid(T))
        {
            Set(std::move(arg));
        }

        Variant(const Variant& arg);
        Variant(Variant&& arg);
        ~Variant();
        Variant& operator=(const Variant& arg);
        Variant& operator=(Variant&& arg);
        bool operator==(const Variant& arg) const;
        bool operator!=(const Variant& arg) const;
        bool operator<(const Variant& arg) const;
        bool operator<=(const Variant& arg) const;
        bool operator>(const Variant& arg) const;
        bool operator>=(const Variant& arg) const;

        void Set();
        template<class T>
        void Set(const T& set);
        template<class T>
        void Set(T&& set);
        void SetMaster(const MasterType& set, ID id);
        void SetMaster(const MasterType&& set, ID id);

        template<class T>
        T& Get();
        template<class T>
        const T& Get() const;
        // Returns nullptr if the type is incorrect
        template<class T>
        T* GetAsPointer();
        // Returns nullptr if the type is incorrect
        template<class T>
        const T* GetAsPointer() const;
        // Does not check that the type you're trying to retrieve is correct
        template<class T>
        T* GetAsRaw();
        // Does not check that the type you're trying to retrieve is correct
        template<class T>
        const T* GetAsRaw() const;
        MasterType GetMaster() const;

        template<class T>
        bool Is() const;
        const std::type_index& GetType() const;
        ID GetTypeAsID() const;
        bool IsInhabited() const;
    private:
        template<class T>
        inline static void CheckForTypeIn()
        {
            static_assert(IsTypeIn<T>::result, "The type you are using must actually be inside the variant.");
        }

        struct NullMasterT
        {};

        enum class VisitStrategy
        {
            DESTRUCTOR,
            EQUALITY,
            NON_EQUALITY,
            LESS,
            LESS_EQUAL,
            GREATER,
            GREATER_EQUAL,

            COPY_CONSTRUCTOR,
            MOVE_CONSTRUCTOR,
            COPY_ASSIGNMENT,
            MOVE_ASSIGNMENT
        };

        class VisitImplementation
        {
        public:
            template<class T>
            static void Do(T& t, StrategySelector<VisitStrategy, VisitStrategy::DESTRUCTOR>)
            {
                t.~T();
            }

            template<class T, class U>
            static bool DoReturn(const T& t, const U& u, StrategySelector<VisitStrategy, VisitStrategy::EQUALITY>)
            {
                return t == u;
            }

            template<class T, class U>
            static bool DoReturn(const T& t, const U& u, StrategySelector<VisitStrategy, VisitStrategy::NON_EQUALITY>)
            {
                return t != u;
            }

            template<class T, class U>
            static bool DoReturn(const T& t, const U& u, StrategySelector<VisitStrategy, VisitStrategy::LESS>)
            {
                return t < u;
            }

            template<class T, class U>
            static bool DoReturn(const T& t, const U& u, StrategySelector<VisitStrategy, VisitStrategy::LESS_EQUAL>)
            {
                return t <= u;
            }

            template<class T, class U>
            static bool DoReturn(const T& t, const U& u, StrategySelector<VisitStrategy, VisitStrategy::GREATER>)
            {
                return t > u;
            }

            template<class T, class U>
            static bool DoReturn(const T& t, const U& u, StrategySelector<VisitStrategy, VisitStrategy::GREATER_EQUAL>)
            {
                return t >= u;
            }

            template<class T>
            static void Do(const T& t, Variant& variant, StrategySelector<VisitStrategy, VisitStrategy::COPY_CONSTRUCTOR>)
            {
                new (&variant.master) T(t);
            }

            template<class T>
            static void Do(T& t, Variant& variant, StrategySelector<VisitStrategy, VisitStrategy::MOVE_CONSTRUCTOR>)
            {
                new (&variant.master) T(std::move(t));
            }

            template<class T>
            static void Do(const T& t, Variant& variant, StrategySelector<VisitStrategy, VisitStrategy::COPY_ASSIGNMENT>)
            {
                new (&variant.master) T(t);
            }

            template<class T>
            static void Do(T& t, Variant& variant, StrategySelector<VisitStrategy, VisitStrategy::MOVE_ASSIGNMENT>)
            {
                new (&variant.master) T(std::move(t));
            }
        };
    private:
        union
        {
            MasterType master;
        };

        std::type_index typeIndex;
    };

    template<class... Args>
    Variant<Args...>::Variant() : typeIndex(typeid(NullMasterT))
    {}

    template<class... Args>
    void Variant<Args...>::Set()
    {
        typeIndex = typeid(NullMasterT);
    }

    template<class... Args>
    template<class T>
    void Variant<Args...>::Set(const T& set)
    {
        CheckForTypeIn<T>();
        new (&master) T(set);
        typeIndex = typeid(T);
    }

    template<class... Args>
    template<class T>
    void Variant<Args...>::Set(T&& set)
    {
        CheckForTypeIn<T>();
        new (&master) T(std::move(set));
        typeIndex = typeid(T);
    }

    template<class... Args>
    void Variant<Args...>::SetMaster(const MasterType& set, ID id)
    {
        new(&master) MasterType(set);
        typeIndex = IterationImplementation::GetTypeIndex(id);
    }

    template<class... Args>
    void Variant<Args...>::SetMaster(const MasterType&& set, ID id)
    {
        new(&master) MasterType(std::move(set));
        typeIndex = IterationImplementation::GetTypeIndex(id);
    }

    template<class... Args>
    template<class T>
    T& Variant<Args...>::Get()
    {
        CheckForTypeIn<T>();
        if (GetType() != typeid(T))
            throw VariantException("The variant is not the same type as requested.");
        return reinterpret_cast<T&>(master);
    }

    template<class... Args>
    template<class T>
    const T& Variant<Args...>::Get() const
    {
        CheckForTypeIn<T>();
        if (GetType() != typeid(T))
            throw VariantException("The variant is not the same type as requested.");
        return reinterpret_cast<const T&>(master);
    }

    template<class... Args>
    template<class T>
    T* Variant<Args...>::GetAsPointer()
    {
        CheckForTypeIn<T>();
        if (GetType() != typeid(T))
            return nullptr;
        return &reinterpret_cast<T&>(master);
    }

    template<class... Args>
    template<class T>
    const T* Variant<Args...>::GetAsPointer() const
    {
        CheckForTypeIn<T>();
        if (GetType() != typeid(T))
            return nullptr;
        return &reinterpret_cast<const T&>(master);
    }

    template<class... Args>
    template<class T>
    T* Variant<Args...>::GetAsRaw()
    {
        CheckForTypeIn<T>();
        return &reinterpret_cast<T&>(master);
    }

    template<class... Args>
    template<class T>
    const T* Variant<Args...>::GetAsRaw() const
    {
        CheckForTypeIn<T>();
        return &reinterpret_cast<const T&>(master);
    }

    template<class... Args>
    typename Variant<Args...>::MasterType Variant<Args...>::GetMaster() const
    {
        return master;
    }

    template<class... Args>
    template<class T>
    bool Variant<Args...>::Is() const
    {
        CheckForTypeIn<T>();
        return std::type_index(typeid(T)) == typeIndex;
    }

    template<class... Args>
    const std::type_index& Variant<Args...>::GetType() const
    {
        return typeIndex;
    }

    template<class... Args>
    typename Variant<Args...>::ID Variant<Args...>::GetTypeAsID() const
    {
        return IterationImplementation::GetID(*this);
    }

    template<class... Args>
    bool Variant<Args...>::IsInhabited() const
    {
        return typeIndex != typeid(NullMasterT);
    }

    template<class T>
    struct VariantTraits
    {
        static constexpr bool isVariant = false;
    };

    template<class... Args>
    struct VariantTraits<Variant<Args...>>
    {
        static constexpr bool isVariant = true;
    };

    template<class... Args>
    struct VariantTraits<const Variant<Args...>>
    {
        static constexpr bool isVariant = true;
    };

    namespace detail
    {
        template<class VariantT, class Use>
        class VariantSwitch
        {
        private:
            typedef typename VariantT::VariadicTemplateT VariadicTemplateT;
        public:
            // Normal
            template<class... Args>
            static void Do(VariantT& variant, Args&& ... args)
            {
                Impl<VariadicTemplateT::count>::Do(variant, std::forward<Args>(args)...);
            }

            // Normal
            template<class Ret, class... Args>
            static Ret DoReturn(VariantT& variant, Args&& ... args)
            {
                return Impl<VariadicTemplateT::count>::DoReturn<Ret>(variant, std::forward<Args>(args)...);
            }

            // Same
            template<class... Args>
            static void DoSame(VariantT& variant1, VariantT& variant2, Args&& ... args)
            {
                Impl<VariadicTemplateT::count>::DoSame(variant1, variant2, std::forward<Args>(args)...);
            }

            // Same
            template<class Ret, class... Args>
            static Ret DoSameReturn(VariantT& variant1, VariantT& variant2, Args&& ... args)
            {
                return Impl<VariadicTemplateT::count>::DoSameReturn<Ret>(variant1, variant2, std::forward<Args>(args)...);
            }
        private:
            // Normal
            template<class T, class... Args>
            static void DoUltimately(VariantT& variant, Args&& ... args)
            {
                Use::Do(variant.Get<T>(), std::forward<Args>(args)...);
            }

            // Normal
            template<class T, class Ret, class... Args>
            static Ret DoUltimatelyReturn(VariantT& variant, Args&& ... args)
            {
                return Use::DoReturn(variant.Get<T>(), std::forward<Args>(args)...);
            }

            // Normal
            template<class T, class... Args>
            static void DoSameUltimately(VariantT& variant1, VariantT& variant2, Args&& ... args)
            {
                Use::Do(variant1.Get<T>(), variant2.Get<T>(), std::forward<Args>(args)...);
            }

            // Normal
            template<class T, class Ret, class... Args>
            static Ret DoSameUltimatelyReturn(VariantT& variant1, VariantT& variant2, Args&& ... args)
            {
                return Use::DoReturn(variant1.Get<T>(), variant2.Get<T>(), std::forward<Args>(args)...);
            }

            template<::function::VariadicTemplateSize index>
            struct Impl
            {
                typedef typename VariadicTemplateT::template Parameter<index - 1>::Type PieceType;

                // Normal
                template<class... Args>
                static void Do(VariantT& variant, Args&& ... args)
                {
                    if (variant.Is<PieceType>())
                        DoUltimately<PieceType>(variant, std::forward<Args>(args)...);
                    else
                        Impl<index - 1>::Do(variant, std::forward<Args>(args)...);
                }

                // Normal
                template<class Ret, class... Args>
                static Ret DoReturn(VariantT& variant, Args&& ... args)
                {
                    if (variant.Is<PieceType>())
                        return DoUltimatelyReturn<PieceType, Ret>(variant, std::forward<Args>(args)...);
                    else
                        return Impl<index - 1>::DoReturn<Ret>(variant, std::forward<Args>(args)...);
                }

                // Same
                template<class... Args>
                static void DoSame(VariantT& variant1, VariantT& variant2, Args&& ... args)
                {
                    if (variant1.Is<PieceType>() && variant2.Is<PieceType>())
                        DoSameUltimately<PieceType>(variant1, variant2, std::forward<Args>(args)...);
                    else
                        Impl<index - 1>::DoSame(variant1, variant2, std::forward<Args>(args)...);
                }

                // Same
                template<class Ret, class... Args>
                static Ret DoSameReturn(VariantT& variant1, VariantT& variant2, Args&& ... args)
                {
                    if (variant1.Is<PieceType>() && variant2.Is<PieceType>())
                        return DoSameUltimatelyReturn<PieceType, Ret>(variant1, variant2, std::forward<Args>(args)...);
                    else
                        return Impl<index - 1>::DoSameReturn<Ret>(variant1, variant2, std::forward<Args>(args)...);
                }
            };

            template<>
            struct Impl<0>
            {
                typedef typename VariadicTemplateT::template Parameter<0>::Type PieceType;

                // Normal
                template<class... Args>
                static void Do(VariantT& variant, Args&& ... args)
                {
                    DoUltimately<PieceType>(variant, std::forward<Args>(args)...);
                }

                // Normal
                template<class Ret, class... Args>
                static Ret DoReturn(VariantT& variant, Args&& ... args)
                {
                    return DoUltimatelyReturn<PieceType, Ret>(variant, std::forward<Args>(args)...);
                }

                // Same
                template<class... Args>
                static void DoSame(VariantT& variant1, VariantT& variant2, Args&& ... args)
                {
                    DoSameUltimately<PieceType>(variant1, variant2, std::forward<Args>(args)...);
                }

                // Same
                template<class Ret, class... Args>
                static Ret DoSameReturn(VariantT& variant1, VariantT& variant2, Args&& ... args)
                {
                    return DoSameUltimatelyReturn<PieceType, Ret>(variant1, variant2, std::forward<Args>(args)...);
                }
            };
        };

        template<class Visitor>
        class MultipleVisitImplementation
        {
        public:
            template<class T, class Variant2, class... Args>
            static void Do(T& t, Variant2& variant2, Args&& ... args);
        };

        template<class Visitor, class Ret>
        class MultipleVisitReturnImplementation
        {
        public:
            template<class T, class Variant2, class... Args>
            static Ret DoReturn(T& t, Variant2& variant2, Args&& ... args);
        };
    }

    // The Visitor needs to have a static Do function with (Type, Args...) arguments
    template<class Visitor, class Variant, class... Args>
    static void Visit(Variant& variant, Args&& ... args)
    {
        static_assert(VariantTraits<Variant>::isVariant, "The variant argument must actually be a Variant.");
        detail::VariantSwitch<Variant, Visitor>::Do(variant, std::forward<Args>(args)...);
    }

    // The Visitor needs to have a static DoReturn function with (Type, Args...) arguments
    template<class Visitor, class Ret, class Variant, class... Args>
    static Ret VisitReturn(Variant& variant, Args&& ... args)
    {
        static_assert(VariantTraits<Variant>::isVariant, "The variant argument must actually be a Variant.");
        return detail::VariantSwitch<Variant, Visitor>::DoReturn<Ret>(variant, std::forward<Args>(args)...);
    }

    // The Visitor needs to have a static Do function with (Type, Args...) arguments
    template<class Visitor, class Variant1, class Variant2, class... Args>
    static void VisitMultiple(Variant1& variant1, Variant2& variant2, Args&& ... args)
    {
        static_assert(VariantTraits<Variant1>::isVariant && VariantTraits<Variant2>::isVariant, "The variant arguments must actually be a Variant.");
        detail::VariantSwitch<Variant2, detail::MultipleVisitImplementation<Visitor>>::Do(variant2, variant1, std::forward<Args>(args)...);
    }

    // The Visitor needs to have a static DoReturn function with (Type, Args...) arguments
    template<class Visitor, class Ret, class Variant1, class Variant2, class... Args>
    static Ret VisitReturnMultiple(Variant1& variant1, Variant2& variant2, Args&& ... args)
    {
        static_assert(VariantTraits<Variant1>::isVariant && VariantTraits<Variant2>::isVariant, "The variant arguments must actually be a Variant.");
        return detail::VariantSwitch<Variant2, detail::MultipleVisitReturnImplementation<Visitor, Ret>>::DoReturn<Ret>(variant2, variant1, std::forward<Args>(args)...);
    }

    // The Visitor needs to have a static Do function with (Type, Args...) arguments
    template<class Visitor, class Variant, class... Args>
    static void VisitMultipleSame(Variant& variant1, Variant& variant2, Args&& ... args)
    {
        detail::VariantSwitch<Variant, Visitor>::DoSame(variant1, variant2, std::forward<Args>(args)...);
    }

    // The Visitor needs to have a static DoReturn function with (Type, Args...) arguments
    template<class Visitor, class Ret, class Variant, class... Args>
    static Ret VisitReturnMultipleSame(Variant& variant1, Variant& variant2, Args&& ... args)
    {
        return detail::VariantSwitch<Variant, Visitor>::DoSameReturn<Ret>(variant1, variant2, std::forward<Args>(args)...);
    }

    template<class... Args>
    Variant<Args...>::Variant(const Variant& arg) : typeIndex(arg.typeIndex)
    {
        if (!arg.IsInhabited())
        {
            typeIndex = typeid(NullMasterT);
            return;
        }

        Visit<VisitImplementation>(arg, *this, StrategySelector<VisitStrategy, VisitStrategy::COPY_CONSTRUCTOR>{});
    }

    template<class... Args>
    Variant<Args...>::Variant(Variant&& arg) : typeIndex(std::move(arg.typeIndex))
    {
        if (!arg.IsInhabited())
        {
            typeIndex = typeid(NullMasterT);
            return;
        }

        Visit<VisitImplementation>(arg, *this, StrategySelector<VisitStrategy, VisitStrategy::MOVE_CONSTRUCTOR>{});
    }

    template<class... Args>
    Variant<Args...>::~Variant()
    {
        if (!IsInhabited())
            return;

        Visit<VisitImplementation>(*this, StrategySelector<VisitStrategy, VisitStrategy::DESTRUCTOR>{});
    }

    template<class... Args>
    Variant<Args...>& Variant<Args...>::operator=(const Variant& arg)
    {
        if (!arg.IsInhabited())
        {
            typeIndex = typeid(NullMasterT);
            return *this;
        }

        Visit<VisitImplementation>(arg, *this, StrategySelector<VisitStrategy, VisitStrategy::COPY_ASSIGNMENT>{});
        typeIndex = arg.typeIndex;

        return *this;
    }

    template<class... Args>
    Variant<Args...>& Variant<Args...>::operator=(Variant&& arg)
    {
        if (!arg.IsInhabited())
        {
            typeIndex = typeid(NullMasterT);
            return *this;
        }

        Visit<VisitImplementation>(arg, *this, StrategySelector<VisitStrategy, VisitStrategy::MOVE_ASSIGNMENT>{});
        typeIndex = std::move(arg.typeIndex);

        return *this;
    }

    template<class... Args>
    bool Variant<Args...>::operator==(const Variant& arg) const
    {
        if (GetType() != arg.GetType())
            return false;

        if (!IsInhabited())
            return true;

        return VisitReturnMultipleSame<VisitImplementation, bool>(*this, arg, StrategySelector<VisitStrategy, VisitStrategy::EQUALITY>{});
    }

    template<class... Args>
    bool Variant<Args...>::operator!=(const Variant& arg) const
    {
        if (GetType() != arg.GetType())
            return true;

        if (IsInhabited() != arg.IsInhabited())
            return true;
        else if (!IsInhabited())
            return false;

        return VisitReturnMultipleSame<VisitImplementation, bool>(*this, arg, StrategySelector<VisitStrategy, VisitStrategy::NON_EQUALITY>{});
    }

    template<class... Args>
    bool Variant<Args...>::operator<(const Variant& arg) const
    {
        if (GetType() != arg.GetType())
            return false;

        if (!IsInhabited())
            return false;

        return VisitReturnMultipleSame<VisitImplementation, bool>(*this, arg, StrategySelector<VisitStrategy, VisitStrategy::LESS>{});
    }

    template<class... Args>
    bool Variant<Args...>::operator<=(const Variant& arg) const
    {
        if (GetType() != arg.GetType())
            return false;

        if (!IsInhabited())
            return false;

        return VisitReturnMultipleSame<VisitImplementation, bool>(*this, arg, StrategySelector<VisitStrategy, VisitStrategy::LESS_EQUAL>{});
    }

    template<class... Args>
    bool Variant<Args...>::operator>(const Variant& arg) const
    {
        if (GetType() != arg.GetType())
            return false;

        if (!IsInhabited())
            return false;

        return VisitReturnMultipleSame<VisitImplementation, bool>(*this, arg, StrategySelector<VisitStrategy, VisitStrategy::GREATER>{});
    }

    template<class... Args>
    bool Variant<Args...>::operator>=(const Variant& arg) const
    {
        if (GetType() != arg.GetType())
            return false;

        if (!IsInhabited())
            return false;

        return VisitReturnMultipleSame<VisitImplementation, bool>(*this, arg, StrategySelector<VisitStrategy, VisitStrategy::GREATER_EQUAL>{});
    }

    namespace detail
    {
        template<class Visitor>
        template<class T, class Variant2, class... Args>
        void MultipleVisitImplementation<Visitor>::Do(T& t, Variant2& variant2, Args&& ... args)
        {
            Visit<Visitor>(variant2, t, std::forward<Args>(args)...);
        }

        template<class Visitor, class Ret>
        template<class T, class Variant2, class... Args>
        Ret MultipleVisitReturnImplementation<Visitor, Ret>::DoReturn(T& t, Variant2& variant2, Args&& ... args)
        {
            return VisitReturn<Visitor, Ret>(variant2, t, std::forward<Args>(args)...);
        }
    }
}