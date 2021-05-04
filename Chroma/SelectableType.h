#pragma once

#include <typeindex>

#include "VariadicTemplate.h"
#include "TypeIdentity.h"

namespace Chroma
{
    namespace detail
    {
        struct NullT
        {};
    }

    template<class... Args>
    class SelectableType
    {
    public:
        using VariadicTemplateT = VariadicTemplate<Args...>;
        using TypeIndex = std::type_index;
        using ID = VariadicTemplateSize;
    public:
        SelectableType();
        template<class T>
        SelectableType(TypeIdentity<T>);
        SelectableType(ID id);
        SelectableType(const SelectableType& arg);
        SelectableType(SelectableType&& arg);

        SelectableType& operator=(const SelectableType& arg);
        SelectableType& operator=(SelectableType&& arg);

        bool operator==(const SelectableType& arg) const;
        bool operator!=(const SelectableType& arg) const;

        template<class T>
        void Select();
        void Select(ID id);
        void Unselect();

        template<class T>
        bool Is() const;
        bool Is(ID id) const;
        bool HasSelected() const;

        const TypeIndex& Selected() const;
        ID SelectedAsID() const;
    private:
        TypeIndex typeIndex;
    private:
        inline static TypeIndex NullType()
        {
            return typeid(detail::NullT);
        }

        template<class T>
        inline static TypeIndex TypeOf()
        {
            return typeid(T);
        }

        inline static TypeIndex TypeOf(ID id)
        {
            return IDImplementation::TypeIndexFromID(id);
        }

        template<class T>
        inline static void AssertTypeIn()
        {
            static_assert(VariadicTemplateT::template IsTypeInside<T>::value, "The type you are using must actually be inside the selectable type.");
        }

        static constexpr VariadicTemplateSize typeCount = VariadicTemplateT::count;
        static_assert(typeCount > 0, "There must be at least one type in this selectable type.");
    private:
        class IDImplementation
        {
        private:
            template<ID index>
            struct Step
            {
                using PieceType = typename VariadicTemplateT::template Parameter<index - 1>::Type;

                static TypeIndex CheckType()
                {
                    return typeid(PieceType);
                }

                static ID IDFrom(const SelectableType& var)
                {
                    if (CheckType() == var.Selected())
                        return index - 1;
                    else
                        return Step<index - 1>::IDFrom(var);
                }

                static TypeIndex TypeIndexFromID(ID id)
                {
                    if (id == index - 1)
                        return CheckType();
                    else
                        return Step<index - 1>::TypeIndexFromID(id);
                }
            };

            template<>
            struct Step<0>
            {
                using PieceType = typename VariadicTemplateT::template Parameter<0>::Type;

                static TypeIndex CheckType()
                {
                    return typeid(PieceType);
                }

                static ID IDFrom(const SelectableType& var)
                {
                    return 0;
                }

                static TypeIndex TypeIndexFromID(ID id)
                {
                    return CheckType();
                }
            };
        public:
            static ID IDFrom(const SelectableType& var) { return Step<typeCount>::IDFrom(var); }
            static TypeIndex TypeIndexFromID(ID id) { return Step<typeCount>::TypeIndexFromID(id); }
        };
    };

    template<class... Args>
    SelectableType<Args...>::SelectableType() : typeIndex(NullType())
    {}

    template<class... Args>
    template<class T>
    SelectableType<Args...>::SelectableType(TypeIdentity<T>) : typeIndex(TypeOf<T>())
    {
        AssertTypeIn<T>();
    }

    template<class... Args>
    SelectableType<Args...>::SelectableType(ID id) : typeIndex(TypeOf(id))
    {}

    template<class... Args>
    SelectableType<Args...>::SelectableType(const SelectableType& arg) : typeIndex(arg.typeIndex)
    {}

    template<class... Args>
    SelectableType<Args...>::SelectableType(SelectableType&& arg) : typeIndex(std::move(arg.typeIndex))
    {}

    template<class... Args>
    SelectableType<Args...>& SelectableType<Args...>::operator=(const SelectableType& arg)
    {
        typeIndex = arg.typeIndex;
        return *this;
    }

    template<class... Args>
    SelectableType<Args...>& SelectableType<Args...>::operator=(SelectableType&& arg)
    {
        typeIndex = std::move(arg.typeIndex);
        return *this;
    }

    template<class... Args>
    bool SelectableType<Args...>::operator==(const SelectableType& arg) const
    {
        return typeIndex == arg.typeIndex;
    }

    template<class... Args>
    bool SelectableType<Args...>::operator!=(const SelectableType& arg) const
    {
        return !(*this == arg);
    }

    template<class... Args>
    template<class T>
    void SelectableType<Args...>::Select()
    {
        AssertTypeIn<T>();

        typeIndex = TypeOf<T>();
    }

    template<class... Args>
    void SelectableType<Args...>::Select(ID id)
    {
        typeIndex = TypeOf(id);
    }

    template<class... Args>
    void SelectableType<Args...>::Unselect()
    {
        typeIndex = NullType();
    }

    template<class... Args>
    template<class T>
    bool SelectableType<Args...>::Is() const
    {
        AssertTypeIn<T>();

        return typeIndex == TypeOf<T>();
    }

    template<class... Args>
    bool SelectableType<Args...>::Is(ID id) const
    {
        return typeIndex == TypeOf(id);
    }

    template<class... Args>
    bool SelectableType<Args...>::HasSelected() const
    {
        return typeIndex != NullType();
    }

    template<class... Args>
    const typename SelectableType<Args...>::TypeIndex& SelectableType<Args...>::Selected() const
    {
        return typeIndex;
    }

    template<class... Args>
    typename SelectableType<Args...>::ID SelectableType<Args...>::SelectedAsID() const
    {
        return IDImplementation::IDFrom(*this);
    }
}