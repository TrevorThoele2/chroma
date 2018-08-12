#pragma once

#include <memory>
#include <typeinfo>

#include "FunctionTraits.h"
#include "Indices.h"
#include "Generator.h"
#include "ParameterPack.h"
#include "Bind.h"
#include "Any.h"

namespace Chroma
{
    template<ParameterIndex i>
    struct Placeholder
    {
        static constexpr ParameterIndex index = i;
        bool operator==(const Placeholder& arg) const { return true; }
    };

    template<class Ret, class... Args>
    class Function
    {
    public:
        Function() = default;
        template<class RealRet>
        Function(RealRet(*func)(Args...));
        template<class RealRet, class Obj, class RealObj>
        Function(RealRet(Obj::*func)(Args...), RealObj* obj);
        template<class RealRet, class Obj, class RealObj>
        Function(RealRet(Obj::*func)(Args...), RealObj& obj);
        template<class RealRet, class Obj, class RealObj>
        Function(RealRet(Obj::*func)(Args...) const, const RealObj* obj);
        template<class RealRet, class Obj, class RealObj>
        Function(RealRet(Obj::*func)(Args...) const, const RealObj& obj);
        // Binders
        template<class RealRet, class... RealArgs, class... BinderArgs>
        Function(RealRet(*func)(RealArgs...), Binder<BinderArgs...>&& bind);
        template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
        Function(RealRet(Obj::*func)(RealArgs...), RealObj* obj, Binder<BinderArgs...>&& bind);
        template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
        Function(RealRet(Obj::*func)(RealArgs...), RealObj& obj, Binder<BinderArgs...>&& bind);
        template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
        Function(RealRet(Obj::*func)(RealArgs...) const, const RealObj* obj, Binder<BinderArgs...>&& bind);
        template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
        Function(RealRet(Obj::*func)(RealArgs...) const, const RealObj& obj, Binder<BinderArgs...>&& bind);

        Function(const Function& arg);
        Function& operator=(const Function& arg);
        Function(Function&& arg);
        Function& operator=(Function&& arg);

        bool operator==(const Function& arg) const;
        bool operator!=(const Function& arg) const;
        explicit operator bool() const;
        operator Function<void, Args...>() const;

        template<class RealRet>
        void Set(RealRet(*func)(Args...));
        template<class RealRet, class Obj, class RealObj>
        void Set(RealRet(Obj::*func)(Args...), RealObj* obj);
        template<class RealRet, class Obj, class RealObj>
        void Set(RealRet(Obj::*func)(Args...), RealObj& obj);
        template<class RealRet, class Obj, class RealObj>
        void Set(RealRet(Obj::*func)(Args...) const, const RealObj* obj);
        template<class RealRet, class Obj, class RealObj>
        void Set(RealRet(Obj::*func)(Args...) const, const RealObj& obj);
        // Binders
        template<class RealRet, class... RealArgs, class... BinderArgs>
        void Set(RealRet(*func)(RealArgs...), Binder<BinderArgs...>&& bind);
        template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
        void Set(RealRet(Obj::*func)(RealArgs...), RealObj* obj, Binder<BinderArgs...>&& bind);
        template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
        void Set(RealRet(Obj::*func)(RealArgs...), RealObj& obj, Binder<BinderArgs...>&& bind);
        template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
        void Set(RealRet(Obj::*func)(RealArgs...) const, const RealObj* obj, Binder<BinderArgs...>&& bind);
        template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
        void Set(RealRet(Obj::*func)(RealArgs...) const, const RealObj& obj, Binder<BinderArgs...>&& bind);

        // Will not fail if called on a non-functor function
        // WILL fail if the object type is not the same as the functor inside here
        template<class Obj>
        bool SetObject(Obj* obj);
        // Will not fail if called on a non-functor function
        // WILL fail if the object type is not the same as the functor inside here
        template<class Obj>
        bool SetObject(Obj& obj);

        // Will not fail if called on a non-functor function
        // WILL fail if the object type is not the same as the functor inside here
        template<class Obj>
        bool SetConstObject(const Obj* obj);
        // Will not fail if called on a non-functor function
        // WILL fail if the object type is not the same as the functor inside here
        template<class Obj>
        bool SetConstObject(const Obj& obj);

        bool IsValid() const;
        bool IsRegularFunction() const;
        bool IsFunctor() const;
        bool IsConstFunctor() const;

        Ret operator()(Args ... args) const;
        Ret Execute(Args ... args) const;
    private:
        typedef VariadicTemplate<Args...> ArgsAsVariadicTemplate;

        template<class BoundT>
        struct BinderTypeForwarder
        {
            typedef BoundT Type;
            static Type Forward(Type t, const typename ArgsAsVariadicTemplate::template ForwardArguments<std::tuple>::Type& passedArgs) { return std::forward<Type>(t); }

            static constexpr bool IsPlaceholder() { return false; }
            static constexpr ParameterIndex index = 0;
        };

        template<ParameterIndex i>
        struct BinderTypeForwarder<Placeholder<i>>
        {
            typedef typename ArgsAsVariadicTemplate::template Parameter<i>::Type Type;
            static Type Forward(Placeholder<i> t, const typename ArgsAsVariadicTemplate::template ForwardArguments<std::tuple>::Type& passedArgs) { return std::get<i>(passedArgs); }

            static constexpr bool IsPlaceholder() { return true; }
            static constexpr ParameterIndex index = i;
        };

        template<ParameterIndex count, class FunctionT, class BoundTupleT>
        struct BinderUnfolder
        {
            typedef BinderTypeForwarder<typename std::tuple_element<count - 1, BoundTupleT>::type> ForwarderT;

            template<class... HoldArgs>
            static Ret Do(FunctionT func, const BoundTupleT& boundArgs, std::tuple<Args...>&& args, HoldArgs&& ... holder) { return BinderUnfolder<count - 1, FunctionT, BoundTupleT>::Do(func, boundArgs, std::move(args), std::forward<typename ForwarderT::Type>(ForwarderT::Forward(std::get<count - 1>(boundArgs), args)), std::forward<HoldArgs>(holder)...); }

            template<class ObjectT, class... HoldArgs>
            static Ret Do(FunctionT func, ObjectT *object, const BoundTupleT& boundArgs, std::tuple<Args...>&& args, HoldArgs&& ... holder) { return BinderUnfolder<count - 1, FunctionT, BoundTupleT>::Do(func, object, boundArgs, std::move(args), std::forward<typename ForwarderT::Type>(ForwarderT::Forward(std::get<count - 1>(boundArgs), args)), std::forward<HoldArgs>(holder)...); }

            template<ParameterIndex placeholderCount>
            static constexpr void CheckPlaceholders()
            {
                BinderUnfolder<count - 1, FunctionT, BoundTupleT>::CheckPlaceholders<(ForwarderT::IsPlaceholder()) ? placeholderCount + 1 : placeholderCount>();
                static_assert(ForwarderT::IsPlaceholder() ? ForwarderT::index <= sizeof...(Args)-1 : true, "A placeholder index is outside the size of the parameter pack.");
            }
        };

        template<class FunctionT, class BoundTupleT>
        struct BinderUnfolder<0, FunctionT, BoundTupleT>
        {
            template<class... HoldArgs>
            static Ret Do(FunctionT func, const BoundTupleT& boundArgs, std::tuple<Args...>&& args, HoldArgs&& ... holder) { return (func)(std::forward<HoldArgs>(holder)...); }

            template<class ObjectT, class... HoldArgs>
            static Ret Do(FunctionT func, ObjectT *object, const BoundTupleT& boundArgs, std::tuple<Args...>&& args, HoldArgs&& ... holder) { return (object->*func)(std::forward<HoldArgs>(holder)...); }

            template<ParameterIndex placeholderCount>
            static constexpr void CheckPlaceholders() { static_assert(placeholderCount >= sizeof...(Args), "The bound argument list is invalid for this function."); }
        };
    private:
        enum class Type
        {
            REGULAR,
            FUNCTOR,
            CONST_FUNCTOR
        };
    private:
        class Base
        {
        public:
            virtual ~Base() = 0 {}

            virtual Base* Clone() const = 0;
            virtual Ret Execute(Args ... args) const = 0;
            virtual Type GetType() const = 0;
            virtual Function<void, Args...> Convert() const = 0;

            virtual bool Compare(const Function& arg) const = 0;
        };

        template<class RealRet>
        class DerivedFunction : public Base
        {
        public:
            typedef RealRet(*Wrapped)(Args...);

            DerivedFunction(Wrapped func);
            DerivedFunction(const DerivedFunction& arg) = default;
            DerivedFunction& operator=(const DerivedFunction& arg) = default;
            DerivedFunction* Clone() const override final;

            Ret Execute(Args ... args) const override final;
            Type GetType() const override final;
            Function<void, Args...> Convert() const override final;

            bool Compare(const Function& arg) const override final;
        private:
            Wrapped func;
        };

        template<class RealRet, class... RealArgs>
        class DerivedBinderFunction
        {
        public:
            template<class... BinderArgs>
            class Impl : public Base
            {
            public:
                typedef RealRet(*Wrapped)(RealArgs...);

                Impl(Wrapped func, Binder<BinderArgs...>&& bind);
                Impl(const Impl& arg) = default;
                Impl& operator=(const Impl& arg) = default;
                Impl* Clone() const override final;

                Ret Execute(Args ... args) const override final;
                Type GetType() const override final;
                Function<void, Args...> Convert() const override final;

                bool Compare(const Function& arg) const override final;
            private:
                Wrapped func;
                typedef std::tuple<BinderArgs...> BoundTupleT;
                BoundTupleT boundArgs;
            };
        };

        template<class RealObj>
        class DerivedFunctorBase : public Base
        {
        public:
            RealObj* obj;
        protected:
            DerivedFunctorBase(RealObj* obj);
        };

        template<class RealRet, class Obj, class RealObj>
        class DerivedFunctor : public DerivedFunctorBase<RealObj>
        {
        public:
            typedef RealRet(Obj::*Wrapped)(Args...);

            DerivedFunctor(Wrapped func, RealObj* obj);
            DerivedFunctor(const DerivedFunctor& arg) = default;
            DerivedFunctor& operator=(const DerivedFunctor& arg) = default;
            DerivedFunctor* Clone() const override final;

            Ret Execute(Args ... args) const override final;
            Type GetType() const override final;
            Function<void, Args...> Convert() const override final;

            bool Compare(const Function& arg) const override final;
        private:
            Wrapped func;
        };

        template<class RealRet, class Obj, class RealObj, class... RealArgs>
        class DerivedBinderFunctor
        {
        public:
            template<class... BinderArgs>
            class Impl : public DerivedFunctorBase<RealObj>
            {
            public:
                typedef RealRet(Obj::*Wrapped)(RealArgs...);

                Impl(Wrapped func, RealObj* obj, Binder<BinderArgs...>&& bind);
                Impl(const Impl& arg) = default;
                Impl& operator=(const Impl& arg) = default;
                Impl* Clone() const override final;

                Ret Execute(Args ... args) const override final;
                Type GetType() const override final;
                Function<void, Args...> Convert() const override final;

                bool Compare(const Function& arg) const override final;
            private:
                Wrapped func;
                typedef std::tuple<BinderArgs...> BoundTupleT;
                BoundTupleT boundArgs;
            };
        };

        template<class RealObj>
        class DerivedConstFunctorBase : public Base
        {
        public:
            const RealObj* obj;
        protected:
            DerivedConstFunctorBase(const RealObj* obj);
        };

        template<class RealRet, class Obj, class RealObj>
        class DerivedConstFunctor : public DerivedConstFunctorBase<RealObj>
        {
        public:
            typedef RealRet(Obj::*Wrapped)(Args...) const;

            DerivedConstFunctor(Wrapped func, const RealObj* obj);
            DerivedConstFunctor(const DerivedConstFunctor& arg) = default;
            DerivedConstFunctor& operator=(const DerivedConstFunctor& arg) = default;
            DerivedConstFunctor* Clone() const override final;

            Ret Execute(Args ... args) const override final;
            Type GetType() const override final;
            Function<void, Args...> Convert() const override final;

            bool Compare(const Function& arg) const override final;
        private:
            Wrapped func;
        };

        template<class RealRet, class Obj, class RealObj, class... RealArgs>
        class DerivedBinderConstFunctor
        {
        public:
            template<class... BinderArgs>
            class Impl : public DerivedConstFunctorBase<RealObj>
            {
            public:
                typedef RealRet(Obj::*Wrapped)(RealArgs...) const;

                Impl(Wrapped func, const RealObj* obj, Binder<BinderArgs...>&& bind);
                Impl(const Impl& arg) = default;
                Impl& operator=(const Impl& arg) = default;
                Impl* Clone() const override final;

                Ret Execute(Args ... args) const override final;
                Type GetType() const override final;
                Function<void, Args...> Convert() const override final;

                bool Compare(const Function& arg) const override final;
            private:
                Wrapped func;
                typedef std::tuple<BinderArgs...> BoundTupleT;
                BoundTupleT boundArgs;
            };
        };
    private:
        typedef std::unique_ptr<Base> BasePtr;
        BasePtr base;
    };

    template<class Ret, class... Args>
    template<class RealRet>
    Function<Ret, Args...>::Function(RealRet(*func)(Args...)) : base(new DerivedFunction<RealRet>(func))
    {}

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    Function<Ret, Args...>::Function(RealRet(Obj::*func)(Args...), RealObj* obj) : base(new DerivedFunctor<RealRet, Obj, RealObj>(func, obj))
    {}

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    Function<Ret, Args...>::Function(RealRet(Obj::*func)(Args...), RealObj& obj) : Function(func, &obj)
    {}

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    Function<Ret, Args...>::Function(RealRet(Obj::*func)(Args...) const, const RealObj* obj) : base(new DerivedConstFunctor<RealRet, Obj, RealObj>(func, obj))
    {}

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    Function<Ret, Args...>::Function(RealRet(Obj::*func)(Args...) const, const RealObj& obj) : Function(func, &obj)
    {}

    template<class Ret, class... Args>
    template<class RealRet, class... RealArgs, class... BinderArgs>
    Function<Ret, Args...>::Function(RealRet(*func)(RealArgs...), Binder<BinderArgs...>&& bind) : base(new typename DerivedBinderFunction<RealRet, RealArgs...>::template Impl<BinderArgs...>(func, std::move(bind)))
    {}

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
    Function<Ret, Args...>::Function(RealRet(Obj::*func)(RealArgs...), RealObj *obj, Binder<BinderArgs...>&& bind) : base(new typename DerivedBinderFunctor<RealRet, Obj, RealObj, RealArgs...>::template Impl<BinderArgs...>(func, obj, std::move(bind)))
    {}

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
    Function<Ret, Args...>::Function(RealRet(Obj::*func)(RealArgs...), RealObj &obj, Binder<BinderArgs...>&& bind) : Function(func, &obj, std::move(bind))
    {}

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
    Function<Ret, Args...>::Function(RealRet(Obj::*func)(RealArgs...) const, const RealObj* obj, Binder<BinderArgs...>&& bind) : base(new typename DerivedBinderConstFunctor<RealRet, Obj, RealObj, RealArgs...>::template Impl<BinderArgs...>(func, obj, std::move(bind)))
    {}

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
    Function<Ret, Args...>::Function(RealRet(Obj::*func)(RealArgs...) const, const RealObj& obj, Binder<BinderArgs...>&& bind) : Function(func, &obj, std::move(bind))
    {}

    template<class Ret, class... Args>
    Function<Ret, Args...>::Function(const Function& arg) : base(arg.base->Clone())
    {}

    template<class Ret, class... Args>
    Function<Ret, Args...>& Function<Ret, Args...>::operator=(const Function& arg)
    {
        base.reset(arg.base->Clone());
        return *this;
    }

    template<class Ret, class... Args>
    Function<Ret, Args...>::Function(Function&& arg) : base(std::move(arg.base))
    {}

    template<class Ret, class... Args>
    Function<Ret, Args...>& Function<Ret, Args...>::operator=(Function&& arg)
    {
        base = std::move(arg.base);
        return *this;
    }

    template<class Ret, class... Args>
    bool Function<Ret, Args...>::operator==(const Function& arg) const
    {
        if (!base || !arg.base)
            return base == arg.base;

        if (typeid(base.get()) != typeid(arg.base.get()))
            return false;

        return base->Compare(arg);
    }

    template<class Ret, class... Args>
    bool Function<Ret, Args...>::operator!=(const Function& arg) const
    {
        return !(*this == arg);
    }

    template<class Ret, class... Args>
    Function<Ret, Args...>::operator bool() const
    {
        return IsValid();
    }

    template<class Ret, class... Args>
    Function<Ret, Args...>::operator Function<void, Args...>() const
    {
        return base->Convert();
    }

    template<class Ret, class... Args>
    template<class RealRet>
    void Function<Ret, Args...>::Set(RealRet(*func)(Args...))
    {
        base.reset(new DerivedFunction<RealRet>(func));
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    void Function<Ret, Args...>::Set(RealRet(Obj::*func)(Args...), RealObj* obj)
    {
        base.reset(new DerivedFunctor<RealRet, Obj, RealObj>(func, obj));
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    void Function<Ret, Args...>::Set(RealRet(Obj::*func)(Args...), RealObj& obj)
    {
        Set(func, &obj);
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    void Function<Ret, Args...>::Set(RealRet(Obj::*func)(Args...) const, const RealObj* obj)
    {
        base.reset(new DerivedConstFunctor<RealRet, Obj, RealObj>(func, obj));
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    void Function<Ret, Args...>::Set(RealRet(Obj::*func)(Args...) const, const RealObj& obj)
    {
        Set(func, &obj);
    }

    template<class Ret, class... Args>
    template<class RealRet, class... RealArgs, class... BinderArgs>
    void Function<Ret, Args...>::Set(RealRet(*func)(RealArgs...), Binder<BinderArgs...>&& bind)
    {
        base.reset(new typename DerivedBinderFunction<RealRet, RealArgs...>::template Impl<BinderArgs...>(func, std::move(bind)));
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
    void Function<Ret, Args...>::Set(RealRet(Obj::*func)(RealArgs...), RealObj* obj, Binder<BinderArgs...>&& bind)
    {
        base.reset(new typename DerivedBinderFunctor<RealRet, Obj, RealObj, RealArgs...>::template Impl<BinderArgs...>(func, obj, std::move(bind)));
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
    void Function<Ret, Args...>::Set(RealRet(Obj::*func)(RealArgs...), RealObj& obj, Binder<BinderArgs...>&& bind)
    {
        Set(func, &obj, std::move(bind));
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
    void Function<Ret, Args...>::Set(RealRet(Obj::*func)(RealArgs...) const, const RealObj* obj, Binder<BinderArgs...>&& bind)
    {
        base.reset(new typename DerivedBinderConstFunctor<RealRet, Obj, RealObj, RealArgs...>::template Impl<BinderArgs...>(func, obj, std::move(bind)));
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
    void Function<Ret, Args...>::Set(RealRet(Obj::*func)(RealArgs...) const, const RealObj& obj, Binder<BinderArgs...>&& bind)
    {
        Set(func, &obj, std::move(bind));
    }

    template<class Ret, class... Args>
    template<class Obj>
    bool Function<Ret, Args...>::SetObject(Obj* obj)
    {
        if (!IsFunctor() || !IsConstFunctor())
            return false;

        static_cast<DerivedFunctorBase<Obj>*>(base.get())->obj = obj;
        return true;
    }

    template<class Ret, class... Args>
    template<class Obj>
    bool Function<Ret, Args...>::SetObject(Obj& obj)
    {
        return SetObject(&obj);
    }

    template<class Ret, class... Args>
    template<class Obj>
    bool Function<Ret, Args...>::SetConstObject(const Obj* obj)
    {
        if (!IsConstFunctor())
            return false;

        static_cast<DerivedConstFunctorBase<Obj>*>(base.get())->obj = obj;
        return true;
    }

    template<class Ret, class... Args>
    template<class Obj>
    bool Function<Ret, Args...>::SetConstObject(const Obj& obj)
    {
        return SetConstObject(&obj);
    }

    template<class Ret, class... Args>
    bool Function<Ret, Args...>::IsValid() const
    {
        return static_cast<bool>(base);
    }

    template<class Ret, class... Args>
    bool Function<Ret, Args...>::IsRegularFunction() const
    {
        return base->GetType() == Type::REGULAR;
    }

    template<class Ret, class... Args>
    bool Function<Ret, Args...>::IsFunctor() const
    {
        return base->GetType() == Type::FUNCTOR;
    }

    template<class Ret, class... Args>
    bool Function<Ret, Args...>::IsConstFunctor() const
    {
        return base->GetType() == Type::CONST_FUNCTOR;
    }

    template<class Ret, class... Args>
    Ret Function<Ret, Args...>::operator()(Args ... args) const
    {
        return Execute(std::forward<Args>(args)...);
    }

    template<class Ret, class... Args>
    Ret Function<Ret, Args...>::Execute(Args ... args) const
    {
        return base->Execute(std::forward<Args>(args)...);
    }

    template<class Ret, class... Args>
    template<class RealRet>
    Function<Ret, Args...>::DerivedFunction<RealRet>::DerivedFunction(Wrapped func) : func(func)
    {}

    template<class Ret, class... Args>
    template<class RealRet>
    typename Function<Ret, Args...>::DerivedFunction<RealRet>* Function<Ret, Args...>::DerivedFunction<RealRet>::Clone() const
    {
        return new DerivedFunction(*this);
    }

    template<class Ret, class... Args>
    template<class RealRet>
    Ret Function<Ret, Args...>::DerivedFunction<RealRet>::Execute(Args ... args) const
    {
        return (func)(std::forward<Args>(args)...);
    }

    template<class Ret, class... Args>
    template<class RealRet>
    typename Function<Ret, Args...>::Type Function<Ret, Args...>::DerivedFunction<RealRet>::GetType() const
    {
        return Type::REGULAR;
    }

    template<class Ret, class... Args>
    template<class RealRet>
    Function<void, Args...> Function<Ret, Args...>::DerivedFunction<RealRet>::Convert() const
    {
        return Function<void, Args...>(func);
    }

    template<class Ret, class... Args>
    template<class RealRet>
    bool Function<Ret, Args...>::DerivedFunction<RealRet>::Compare(const Function& arg) const
    {
        DerivedFunction* convertedArg(static_cast<DerivedFunction*>(arg.base.get()));
        return func == convertedArg->func;
    }

    template<class Ret, class... Args>
    template<class RealRet, class... RealArgs>
    template<class... BinderArgs>
    Function<Ret, Args...>::DerivedBinderFunction<RealRet, RealArgs...>::Impl<BinderArgs...>::Impl(Wrapped func, Binder<BinderArgs...>&& bind) : func(func), boundArgs(std::move(bind.tuple))
    {}

    template<class Ret, class... Args>
    template<class RealRet, class... RealArgs>
    template<class... BinderArgs>
    Function<Ret, Args...>::DerivedBinderFunction<RealRet, RealArgs...>::Impl<BinderArgs...>* Function<Ret, Args...>::DerivedBinderFunction<RealRet, RealArgs...>::Impl<BinderArgs...>::Clone() const
    {
        return new Impl(*this);
    }

    template<class Ret, class... Args>
    template<class RealRet, class... RealArgs>
    template<class... BinderArgs>
    Ret Function<Ret, Args...>::DerivedBinderFunction<RealRet, RealArgs...>::Impl<BinderArgs...>::Execute(Args ... args) const
    {
        BinderUnfolder<sizeof...(BinderArgs), Wrapped, BoundTupleT>::CheckPlaceholders<0>();
        return BinderUnfolder<sizeof...(BinderArgs), Wrapped, BoundTupleT>::Do(func, boundArgs, std::tuple<Args...>(std::forward<Args>(args)...));
    }

    template<class Ret, class... Args>
    template<class RealRet, class... RealArgs>
    template<class... BinderArgs>
    typename Function<Ret, Args...>::Type Function<Ret, Args...>::DerivedBinderFunction<RealRet, RealArgs...>::Impl<BinderArgs...>::GetType() const
    {
        return Type::REGULAR;
    }

    template<class Ret, class... Args>
    template<class RealRet, class... RealArgs>
    template<class... BinderArgs>
    Function<void, Args...> Function<Ret, Args...>::DerivedBinderFunction<RealRet, RealArgs...>::Impl<BinderArgs...>::Convert() const
    {
        return Function<void, Args...>(func, Binder<BinderArgs...>(BoundTupleT(boundArgs)));
    }

    template<class Ret, class... Args>
    template<class RealRet, class... RealArgs>
    template<class... BinderArgs>
    bool Function<Ret, Args...>::DerivedBinderFunction<RealRet, RealArgs...>::Impl<BinderArgs...>::Compare(const Function& arg) const
    {
        Impl* convertedArg(static_cast<Impl*>(arg.base.get()));
        return func == convertedArg->func && boundArgs == convertedArg->boundArgs;
    }

    template<class Ret, class... Args>
    template<class RealObj>
    Function<Ret, Args...>::DerivedFunctorBase<RealObj>::DerivedFunctorBase(RealObj* obj) : obj(obj)
    {}

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    Function<Ret, Args...>::DerivedFunctor<RealRet, Obj, RealObj>::DerivedFunctor(Wrapped func, RealObj* obj) : DerivedFunctorBase(obj), func(func)
    {}

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    typename Function<Ret, Args...>::DerivedFunctor<RealRet, Obj, RealObj>* Function<Ret, Args...>::DerivedFunctor<RealRet, Obj, RealObj>::Clone() const
    {
        return new DerivedFunctor(*this);
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    Ret Function<Ret, Args...>::DerivedFunctor<RealRet, Obj, RealObj>::Execute(Args ... args) const
    {
        return (obj->*func)(std::forward<Args>(args)...);
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    typename Function<Ret, Args...>::Type Function<Ret, Args...>::DerivedFunctor<RealRet, Obj, RealObj>::GetType() const
    {
        return Type::FUNCTOR;
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    Function<void, Args...> Function<Ret, Args...>::DerivedFunctor<RealRet, Obj, RealObj>::Convert() const
    {
        return Function<void, Args...>(func, obj);
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    bool Function<Ret, Args...>::DerivedFunctor<RealRet, Obj, RealObj>::Compare(const Function& arg) const
    {
        DerivedFunctor* convertedArg(static_cast<DerivedFunctor*>(arg.base.get()));
        return func == convertedArg->func && obj == convertedArg->obj;
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    Function<Ret, Args...>::DerivedBinderFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Impl(Wrapped func, RealObj* obj, Binder<BinderArgs...>&& bind) : DerivedFunctorBase(obj), func(func), boundArgs(std::move(bind.tuple))
    {}

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    Function<Ret, Args...>::DerivedBinderFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>* Function<Ret, Args...>::DerivedBinderFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Clone() const
    {
        return new Impl(*this);
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    Ret Function<Ret, Args...>::DerivedBinderFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Execute(Args ... args) const
    {
        BinderUnfolder<sizeof...(BinderArgs), Wrapped, BoundTupleT>::CheckPlaceholders<0>();
        return BinderUnfolder<sizeof...(BinderArgs), Wrapped, BoundTupleT>::Do(func, obj, boundArgs, std::tuple<Args...>(std::forward<Args>(args)...));
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    typename Function<Ret, Args...>::Type Function<Ret, Args...>::DerivedBinderFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::GetType() const
    {
        return Type::FUNCTOR;
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    Function<void, Args...> Function<Ret, Args...>::DerivedBinderFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Convert() const
    {
        return Function<void, Args...>(func, obj, Binder<BinderArgs...>(BoundTupleT(boundArgs)));
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    bool Function<Ret, Args...>::DerivedBinderFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Compare(const Function& arg) const
    {
        Impl* convertedArg(static_cast<Impl*>(arg.base.get()));
        return func == convertedArg->func && obj == convertedArg->obj && boundArgs == convertedArg->boundArgs;
    }

    template<class Ret, class... Args>
    template<class RealObj>
    Function<Ret, Args...>::DerivedConstFunctorBase<RealObj>::DerivedConstFunctorBase(const RealObj* obj) : obj(obj)
    {}

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    Function<Ret, Args...>::DerivedConstFunctor<RealRet, Obj, RealObj>::DerivedConstFunctor(Wrapped func, const RealObj* obj) : DerivedConstFunctorBase(obj), func(func)
    {}

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    typename Function<Ret, Args...>::DerivedConstFunctor<RealRet, Obj, RealObj>* Function<Ret, Args...>::DerivedConstFunctor<RealRet, Obj, RealObj>::Clone() const
    {
        return new DerivedConstFunctor(*this);
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    Ret Function<Ret, Args...>::DerivedConstFunctor<RealRet, Obj, RealObj>::Execute(Args ... args) const
    {
        return (obj->*func)(std::forward<Args>(args)...);
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    typename Function<Ret, Args...>::Type Function<Ret, Args...>::DerivedConstFunctor<RealRet, Obj, RealObj>::GetType() const
    {
        return Type::CONST_FUNCTOR;
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    Function<void, Args...> Function<Ret, Args...>::DerivedConstFunctor<RealRet, Obj, RealObj>::Convert() const
    {
        return Function<void, Args...>(func, obj);
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj>
    bool Function<Ret, Args...>::DerivedConstFunctor<RealRet, Obj, RealObj>::Compare(const Function& arg) const
    {
        DerivedConstFunctor* convertedArg(static_cast<DerivedConstFunctor*>(arg.base.get()));
        return func == convertedArg->func && obj == convertedArg->obj;
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    Function<Ret, Args...>::DerivedBinderConstFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Impl(Wrapped func, const RealObj* obj, Binder<BinderArgs...>&& bind) : DerivedConstFunctorBase(obj), func(func), boundArgs(std::move(bind.tuple))
    {}

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    Function<Ret, Args...>::DerivedBinderConstFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>* Function<Ret, Args...>::DerivedBinderConstFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Clone() const
    {
        return new Impl(*this);
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    Ret Function<Ret, Args...>::DerivedBinderConstFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Execute(Args ... args) const
    {
        BinderUnfolder<sizeof...(BinderArgs), Wrapped, BoundTupleT>::CheckPlaceholders<0>();
        return BinderUnfolder<sizeof...(BinderArgs), Wrapped, BoundTupleT>::Do(func, obj, boundArgs, std::tuple<Args...>(std::forward<Args>(args)...));
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    typename Function<Ret, Args...>::Type Function<Ret, Args...>::DerivedBinderConstFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::GetType() const
    {
        return Type::CONST_FUNCTOR;
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    Function<void, Args...> Function<Ret, Args...>::DerivedBinderConstFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Convert() const
    {
        return Function<void, Args...>(func, obj, Binder<BinderArgs...>(BoundTupleT(boundArgs)));
    }

    template<class Ret, class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    bool Function<Ret, Args...>::DerivedBinderConstFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Compare(const Function& arg) const
    {
        Impl* convertedArg(static_cast<Impl*>(arg.base.get()));
        return func == convertedArg->func && obj == convertedArg->obj && boundArgs == convertedArg->boundArgs;
    }

    template<class... Args>
    class Function<void, Args...>
    {
    public:
        Function() = default;
        template<class RealRet>
        Function(RealRet(*func)(Args...));
        template<class RealRet, class Obj, class RealObj>
        Function(RealRet(Obj::*func)(Args...), RealObj* obj);
        template<class RealRet, class Obj, class RealObj>
        Function(RealRet(Obj::*func)(Args...), RealObj& obj);
        template<class RealRet, class Obj, class RealObj>
        Function(RealRet(Obj::*func)(Args...) const, const RealObj* obj);
        template<class RealRet, class Obj, class RealObj>
        Function(RealRet(Obj::*func)(Args...) const, const RealObj& obj);
        // Binders
        template<class RealRet, class... RealArgs, class... BinderArgs>
        Function(RealRet(*func)(RealArgs...), Binder<BinderArgs...>&& bind);
        template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
        Function(RealRet(Obj::*func)(RealArgs...), RealObj* obj, Binder<BinderArgs...>&& bind);
        template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
        Function(RealRet(Obj::*func)(RealArgs...), RealObj& obj, Binder<BinderArgs...>&& bind);
        template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
        Function(RealRet(Obj::*func)(RealArgs...) const, const RealObj* obj, Binder<BinderArgs...>&& bind);
        template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
        Function(RealRet(Obj::*func)(RealArgs...) const, const RealObj& obj, Binder<BinderArgs...>&& bind);

        Function(const Function& arg);
        Function& operator=(const Function& arg);
        Function(Function&& arg);
        Function& operator=(Function&& arg);

        bool operator==(const Function& arg) const;
        bool operator!=(const Function& arg) const;
        explicit operator bool() const;

        template<class RealRet>
        void Set(RealRet(*func)(Args...));
        template<class RealRet, class Obj, class RealObj>
        void Set(RealRet(Obj::*func)(Args...), RealObj* obj);
        template<class RealRet, class Obj, class RealObj>
        void Set(RealRet(Obj::*func)(Args...), RealObj& obj);
        template<class RealRet, class Obj, class RealObj>
        void Set(RealRet(Obj::*func)(Args...) const, const RealObj* obj);
        template<class RealRet, class Obj, class RealObj>
        void Set(RealRet(Obj::*func)(Args...) const, const RealObj& obj);
        // Binders
        template<class RealRet, class... RealArgs, class... BinderArgs>
        void Set(RealRet(*func)(RealArgs...), Binder<BinderArgs...>&& bind);
        template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
        void Set(RealRet(Obj::*func)(RealArgs...), RealObj* obj, Binder<BinderArgs...>&& bind);
        template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
        void Set(RealRet(Obj::*func)(RealArgs...), RealObj& obj, Binder<BinderArgs...>&& bind);
        template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
        void Set(RealRet(Obj::*func)(RealArgs...) const, const RealObj* obj, Binder<BinderArgs...>&& bind);
        template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
        void Set(RealRet(Obj::*func)(RealArgs...) const, const RealObj& obj, Binder<BinderArgs...>&& bind);

        // Will not fail if called on a non-functor function
        // WILL fail if the object type is not the same as the functor inside here
        template<class Obj>
        bool SetObject(Obj* obj);
        // Will not fail if called on a non-functor function
        // WILL fail if the object type is not the same as the functor inside here
        template<class Obj>
        bool SetObject(Obj& obj);

        // Will not fail if called on a non-functor function
        // WILL fail if the object type is not the same as the functor inside here
        template<class Obj>
        bool SetConstObject(const Obj* obj);
        // Will not fail if called on a non-functor function
        // WILL fail if the object type is not the same as the functor inside here
        template<class Obj>
        bool SetConstObject(const Obj& obj);

        bool IsValid() const;
        bool IsRegularFunction() const;
        bool IsFunctor() const;
        bool IsConstFunctor() const;

        void operator()(Args ... args) const;
        void Execute(Args ... args) const;
    private:
        typedef VariadicTemplate<Args...> ArgsAsVariadicTemplate;

        template<class BoundT>
        struct BinderTypeForwarder
        {
            typedef BoundT Type;
            static Type Forward(Type t, const typename ArgsAsVariadicTemplate::template ForwardArguments<std::tuple>::Type& passedArgs) { return std::forward<Type>(t); }

            static constexpr bool IsPlaceholder() { return false; }
            static constexpr ParameterIndex index = 0;
        };

        template<ParameterIndex i>
        struct BinderTypeForwarder<Placeholder<i>>
        {
            typedef typename ArgsAsVariadicTemplate::template Parameter<i>::Type Type;
            static Type Forward(Placeholder<i> t, const typename ArgsAsVariadicTemplate::template ForwardArguments<std::tuple>::Type& passedArgs) { return std::get<i>(passedArgs); }

            static constexpr bool IsPlaceholder() { return true; }
            static constexpr ParameterIndex index = i;
        };

        template<ParameterIndex count, class FunctionT, class BoundTupleT>
        struct BinderUnfolder
        {
            typedef BinderTypeForwarder<typename std::tuple_element<count - 1, BoundTupleT>::type> ForwarderT;

            template<class... HoldArgs>
            static void Do(FunctionT func, const BoundTupleT& boundArgs, std::tuple<Args...>&& args, HoldArgs&& ... holder) { BinderUnfolder<count - 1, FunctionT, BoundTupleT>::Do(func, boundArgs, std::move(args), std::forward<typename ForwarderT::Type>(ForwarderT::Forward(std::get<count - 1>(boundArgs), args)), std::forward<HoldArgs>(holder)...); }

            template<class ObjectT, class... HoldArgs>
            static void Do(FunctionT func, ObjectT *object, const BoundTupleT& boundArgs, std::tuple<Args...>&& args, HoldArgs&& ... holder) { BinderUnfolder<count - 1, FunctionT, BoundTupleT>::Do(func, object, boundArgs, std::move(args), std::forward<typename ForwarderT::Type>(ForwarderT::Forward(std::get<count - 1>(boundArgs), args)), std::forward<HoldArgs>(holder)...); }

            template<ParameterIndex placeholderCount>
            static constexpr void CheckPlaceholders()
            {
                BinderUnfolder<count - 1, FunctionT, BoundTupleT>::CheckPlaceholders<(ForwarderT::IsPlaceholder()) ? placeholderCount + 1 : placeholderCount>();
                static_assert(ForwarderT::IsPlaceholder() ? ForwarderT::index <= sizeof...(Args)-1 : true, "A placeholder index is outside the size of the parameter pack.");
            }
        };

        template<class FunctionT, class BoundTupleT>
        struct BinderUnfolder<0, FunctionT, BoundTupleT>
        {
            template<class... HoldArgs>
            static void Do(FunctionT func, const BoundTupleT& boundArgs, std::tuple<Args...>&& args, HoldArgs&& ... holder) { (func)(std::forward<HoldArgs>(holder)...); }

            template<class ObjectT, class... HoldArgs>
            static void Do(FunctionT func, ObjectT *object, const BoundTupleT& boundArgs, std::tuple<Args...>&& args, HoldArgs&& ... holder) { (object->*func)(std::forward<HoldArgs>(holder)...); }

            template<ParameterIndex placeholderCount>
            static constexpr void CheckPlaceholders() { static_assert(placeholderCount >= sizeof...(Args), "The bound argument list is invalid for this function."); }
        };
    private:
        enum class Type
        {
            REGULAR,
            FUNCTOR,
            CONST_FUNCTOR
        };
    private:
        class Base
        {
        public:
            virtual ~Base() = 0 {}

            virtual Base* Clone() const = 0;
            virtual void Execute(Args ... args) const = 0;
            virtual Type GetType() const = 0;
            virtual bool Compare(const Function& arg) const = 0;
        };

        template<class RealRet>
        class DerivedFunction : public Base
        {
        public:
            typedef RealRet(*Wrapped)(Args...);

            DerivedFunction(Wrapped func);
            DerivedFunction(const DerivedFunction& arg) = default;
            DerivedFunction& operator=(const DerivedFunction& arg) = default;
            DerivedFunction* Clone() const override final;

            void Execute(Args ... args) const override final;
            Type GetType() const override final;
            bool Compare(const Function& arg) const override final;
        private:
            Wrapped func;
        };

        template<class RealRet, class... RealArgs>
        class DerivedBinderFunction
        {
        public:
            template<class... BinderArgs>
            class Impl : public Base
            {
            public:
                typedef RealRet(*Wrapped)(RealArgs...);

                Impl(Wrapped func, Binder<BinderArgs...>&& bind);
                Impl(const Impl& arg) = default;
                Impl& operator=(const Impl& arg) = default;
                Impl* Clone() const override final;

                void Execute(Args ... args) const override final;
                Type GetType() const override final;
                bool Compare(const Function& arg) const override final;
            private:
                Wrapped func;
                typedef std::tuple<BinderArgs...> BoundTupleT;
                BoundTupleT boundArgs;
            };
        };

        template<class RealObj>
        class DerivedFunctorBase : public Base
        {
        public:
            RealObj* obj;
        protected:
            DerivedFunctorBase(RealObj* obj);
        };

        template<class RealRet, class Obj, class RealObj>
        class DerivedFunctor : public DerivedFunctorBase<RealObj>
        {
        public:
            typedef RealRet(Obj::*Wrapped)(Args...);

            DerivedFunctor(Wrapped func, RealObj* obj);
            DerivedFunctor(const DerivedFunctor& arg) = default;
            DerivedFunctor& operator=(const DerivedFunctor& arg) = default;
            DerivedFunctor* Clone() const override final;

            void Execute(Args ... args) const override final;
            Type GetType() const override final;
            bool Compare(const Function& arg) const override final;
        private:
            Wrapped func;
        };

        template<class RealRet, class Obj, class RealObj, class... RealArgs>
        class DerivedBinderFunctor
        {
        public:
            template<class... BinderArgs>
            class Impl : public DerivedFunctorBase<Obj>
            {
            public:
                typedef RealRet(Obj::*Wrapped)(RealArgs...);

                Impl(Wrapped func, RealObj* obj, Binder<BinderArgs...>&& bind);
                Impl(const Impl& arg) = default;
                Impl& operator=(const Impl& arg) = default;
                Impl* Clone() const override final;

                void Execute(Args ... args) const override final;
                Type GetType() const override final;
                bool Compare(const Function& arg) const override final;
            private:
                Wrapped func;
                typedef std::tuple<BinderArgs...> BoundTupleT;
                BoundTupleT boundArgs;
            };
        };

        template<class RealObj>
        class DerivedConstFunctorBase : public Base
        {
        public:
            const RealObj* obj;
        protected:
            DerivedConstFunctorBase(const RealObj* obj);
        };

        template<class RealRet, class Obj, class RealObj>
        class DerivedConstFunctor : public DerivedConstFunctorBase<RealObj>
        {
        public:
            typedef RealRet(Obj::*Wrapped)(Args...) const;

            DerivedConstFunctor(Wrapped func, const RealObj* obj);
            DerivedConstFunctor(const DerivedConstFunctor& arg) = default;
            DerivedConstFunctor& operator=(const DerivedConstFunctor& arg) = default;
            DerivedConstFunctor* Clone() const override final;

            void Execute(Args ... args) const override final;
            Type GetType() const override final;
            bool Compare(const Function& arg) const override final;
        private:
            Wrapped func;
        };

        template<class RealRet, class Obj, class RealObj, class... RealArgs>
        class DerivedBinderConstFunctor
        {
        public:
            template<class... BinderArgs>
            class Impl : public DerivedConstFunctorBase<Obj>
            {
            public:
                typedef RealRet(Obj::*Wrapped)(RealArgs...) const;

                Impl(Wrapped func, const RealObj* obj, Binder<BinderArgs...>&& bind);
                Impl(const Impl& arg) = default;
                Impl& operator=(const Impl& arg) = default;
                Impl* Clone() const override final;

                void Execute(Args ... args) const override final;
                Type GetType() const override final;
                bool Compare(const Function& arg) const override final;
            private:
                Wrapped func;
                typedef std::tuple<BinderArgs...> BoundTupleT;
                BoundTupleT boundArgs;
            };
        };
    private:
        typedef std::unique_ptr<Base> BasePtr;
        BasePtr base;
    };

    template<class... Args>
    template<class RealRet>
    Function<void, Args...>::Function(RealRet(*func)(Args...)) : base(new DerivedFunction<RealRet>(func))
    {}

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    Function<void, Args...>::Function(RealRet(Obj::*func)(Args...), RealObj* obj) : base(new DerivedFunctor<RealRet, Obj, RealObj>(func, obj))
    {}

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    Function<void, Args...>::Function(RealRet(Obj::*func)(Args...), RealObj& obj) : Function(func, &obj)
    {}

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    Function<void, Args...>::Function(RealRet(Obj::*func)(Args...) const, const RealObj* obj) : base(new DerivedConstFunctor<RealRet, Obj, RealObj>(func, obj))
    {}

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    Function<void, Args...>::Function(RealRet(Obj::*func)(Args...) const, const RealObj& obj) : Function(func, &obj)
    {}

    template<class... Args>
    template<class RealRet, class... RealArgs, class... BinderArgs>
    Function<void, Args...>::Function(RealRet(*func)(RealArgs...), Binder<BinderArgs...>&& bind) : base(new typename DerivedBinderFunction<RealRet, RealArgs...>::template Impl<BinderArgs...>(func, std::move(bind)))
    {}

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
    Function<void, Args...>::Function(RealRet(Obj::*func)(RealArgs...), RealObj *obj, Binder<BinderArgs...>&& bind) : base(new typename DerivedBinderFunctor<RealRet, Obj, RealObj, RealArgs...>::template Impl<BinderArgs...>(func, obj, std::move(bind)))
    {}

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
    Function<void, Args...>::Function(RealRet(Obj::*func)(RealArgs...), RealObj &obj, Binder<BinderArgs...>&& bind) : Function(func, &obj, std::move(bind))
    {}

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
    Function<void, Args...>::Function(RealRet(Obj::*func)(RealArgs...) const, const RealObj* obj, Binder<BinderArgs...>&& bind) : base(new typename DerivedBinderConstFunctor<RealRet, Obj, RealObj, RealArgs...>::template Impl<BinderArgs...>(func, obj, std::move(bind)))
    {}

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
    Function<void, Args...>::Function(RealRet(Obj::*func)(RealArgs...) const, const RealObj& obj, Binder<BinderArgs...>&& bind) : Function(func, &obj, std::move(bind))
    {}

    template<class... Args>
    Function<void, Args...>::Function(const Function& arg) : base(arg.base->Clone())
    {}

    template<class... Args>
    Function<void, Args...>& Function<void, Args...>::operator=(const Function& arg)
    {
        base.reset(arg.base->Clone());
        return *this;
    }

    template<class... Args>
    Function<void, Args...>::Function(Function&& arg) : base(std::move(arg.base))
    {}

    template<class... Args>
    Function<void, Args...>& Function<void, Args...>::operator=(Function&& arg)
    {
        base = std::move(arg.base);
        return *this;
    }

    template<class... Args>
    bool Function<void, Args...>::operator==(const Function& arg) const
    {
        if (!base || !arg.base)
            return base == arg.base;

        if (typeid(base.get()) != typeid(arg.base.get()))
            return false;

        return base->Compare(arg);
    }

    template<class... Args>
    bool Function<void, Args...>::operator!=(const Function& arg) const
    {
        return !(*this == arg);
    }

    template<class... Args>
    Function<void, Args...>::operator bool() const
    {
        return IsValid();
    }

    template<class... Args>
    template<class RealRet>
    void Function<void, Args...>::Set(RealRet(*func)(Args...))
    {
        base.reset(new DerivedFunction<RealRet>(func));
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    void Function<void, Args...>::Set(RealRet(Obj::*func)(Args...), RealObj* obj)
    {
        base.reset(new DerivedFunctor<RealRet, Obj>(func, obj));
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    void Function<void, Args...>::Set(RealRet(Obj::*func)(Args...), RealObj& obj)
    {
        Set(func, &obj);
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    void Function<void, Args...>::Set(RealRet(Obj::*func)(Args...) const, const RealObj* obj)
    {
        base.reset(new DerivedConstFunctor<RealRet, Obj>(func, obj));
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    void Function<void, Args...>::Set(RealRet(Obj::*func)(Args...) const, const RealObj& obj)
    {
        Set(func, &obj);
    }

    template<class... Args>
    template<class RealRet, class... RealArgs, class... BinderArgs>
    void Function<void, Args...>::Set(RealRet(*func)(RealArgs...), Binder<BinderArgs...>&& bind)
    {
        base.reset(new typename DerivedBinderFunction<RealRet, RealArgs...>::template Impl<BinderArgs...>(func, std::move(bind)));
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
    void Function<void, Args...>::Set(RealRet(Obj::*func)(RealArgs...), RealObj* obj, Binder<BinderArgs...>&& bind)
    {
        base.reset(new typename DerivedBinderFunctor<RealRet, Obj, RealArgs...>::template Impl<BinderArgs...>(func, obj, std::move(bind)));
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
    void Function<void, Args...>::Set(RealRet(Obj::*func)(RealArgs...), RealObj& obj, Binder<BinderArgs...>&& bind)
    {
        Set(func, &obj, std::move(bind));
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
    void Function<void, Args...>::Set(RealRet(Obj::*func)(RealArgs...) const, const RealObj* obj, Binder<BinderArgs...>&& bind)
    {
        base.reset(new typename DerivedBinderConstFunctor<RealRet, Obj, RealArgs...>::template Impl<BinderArgs...>(func, obj, std::move(bind)));
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs, class... BinderArgs>
    void Function<void, Args...>::Set(RealRet(Obj::*func)(RealArgs...) const, const RealObj& obj, Binder<BinderArgs...>&& bind)
    {
        Set(func, &obj, std::move(bind));
    }

    template<class... Args>
    template<class Obj>
    bool Function<void, Args...>::SetObject(Obj* obj)
    {
        if (!IsFunctor() || !IsConstFunctor())
            return false;

        static_cast<DerivedFunctorBase<Obj>*>(base.get())->obj = obj;
        return true;
    }

    template<class... Args>
    template<class Obj>
    bool Function<void, Args...>::SetObject(Obj& obj)
    {
        return SetObject(&obj);
    }

    template<class... Args>
    template<class Obj>
    bool Function<void, Args...>::SetConstObject(const Obj* obj)
    {
        if (!IsConstFunctor())
            return false;

        static_cast<DerivedConstFunctorBase<Obj>*>(base.get())->obj = obj;
        return true;
    }

    template<class... Args>
    template<class Obj>
    bool Function<void, Args...>::SetConstObject(const Obj& obj)
    {
        return SetConstObject(&obj);
    }

    template<class... Args>
    bool Function<void, Args...>::IsValid() const
    {
        return static_cast<bool>(base);
    }

    template<class... Args>
    bool Function<void, Args...>::IsRegularFunction() const
    {
        return base->GetType() == Type::REGULAR;
    }

    template<class... Args>
    bool Function<void, Args...>::IsFunctor() const
    {
        return base->GetType() == Type::FUNCTOR;
    }

    template<class... Args>
    bool Function<void, Args...>::IsConstFunctor() const
    {
        return base->GetType() == Type::CONST_FUNCTOR;
    }

    template<class... Args>
    void Function<void, Args...>::operator()(Args ... args) const
    {
       Execute(std::forward<Args>(args)...);
    }

    template<class... Args>
    void Function<void, Args...>::Execute(Args ... args) const
    {
        base->Execute(std::forward<Args>(args)...);
    }

    template<class... Args>
    template<class RealRet>
    Function<void, Args...>::DerivedFunction<RealRet>::DerivedFunction(Wrapped func) : func(func)
    {}

    template<class... Args>
    template<class RealRet>
    typename Function<void, Args...>::DerivedFunction<RealRet>* Function<void, Args...>::DerivedFunction<RealRet>::Clone() const
    {
        return new DerivedFunction(*this);
    }

    template<class... Args>
    template<class RealRet>
    void Function<void, Args...>::DerivedFunction<RealRet>::Execute(Args ... args) const
    {
        (func)(std::forward<Args>(args)...);
    }

    template<class... Args>
    template<class RealRet>
    typename Function<void, Args...>::Type Function<void, Args...>::DerivedFunction<RealRet>::GetType() const
    {
        return Type::REGULAR;
    }

    template<class... Args>
    template<class RealRet>
    bool Function<void, Args...>::DerivedFunction<RealRet>::Compare(const Function& arg) const
    {
        DerivedFunction* convertedArg(static_cast<DerivedFunction*>(arg.base.get()));
        return func == convertedArg->func;
    }

    template<class... Args>
    template<class RealRet, class... RealArgs>
    template<class... BinderArgs>
    Function<void, Args...>::DerivedBinderFunction<RealRet, RealArgs...>::Impl<BinderArgs...>::Impl(Wrapped func, Binder<BinderArgs...>&& bind) : func(func), boundArgs(std::move(bind.tuple))
    {}

    template<class... Args>
    template<class RealRet, class... RealArgs>
    template<class... BinderArgs>
    Function<void, Args...>::DerivedBinderFunction<RealRet, RealArgs...>::Impl<BinderArgs...>* Function<void, Args...>::DerivedBinderFunction<RealRet, RealArgs...>::Impl<BinderArgs...>::Clone() const
    {
        return new Impl(*this);
    }

    template<class... Args>
    template<class RealRet, class... RealArgs>
    template<class... BinderArgs>
    void Function<void, Args...>::DerivedBinderFunction<RealRet, RealArgs...>::Impl<BinderArgs...>::Execute(Args ... args) const
    {
        BinderUnfolder<sizeof...(BinderArgs), Wrapped, BoundTupleT>::CheckPlaceholders<0>();
        BinderUnfolder<sizeof...(BinderArgs), Wrapped, BoundTupleT>::Do(func, boundArgs, std::tuple<Args...>(std::forward<Args>(args)...));
    }

    template<class... Args>
    template<class RealRet, class... RealArgs>
    template<class... BinderArgs>
    typename Function<void, Args...>::Type Function<void, Args...>::DerivedBinderFunction<RealRet, RealArgs...>::Impl<BinderArgs...>::GetType() const
    {
        return Type::REGULAR;
    }

    template<class... Args>
    template<class RealRet, class... RealArgs>
    template<class... BinderArgs>
    bool Function<void, Args...>::DerivedBinderFunction<RealRet, RealArgs...>::Impl<BinderArgs...>::Compare(const Function& arg) const
    {
        Impl* convertedArg(static_cast<Impl*>(arg.base.get()));
        return func == convertedArg->func && boundArgs == convertedArg->boundArgs;
    }

    template<class... Args>
    template<class RealObj>
    Function<void, Args...>::DerivedFunctorBase<RealObj>::DerivedFunctorBase(RealObj* obj) : obj(obj)
    {}

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    Function<void, Args...>::DerivedFunctor<RealRet, Obj, RealObj>::DerivedFunctor(Wrapped func, RealObj* obj) : DerivedFunctorBase(obj), func(func)
    {}

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    typename Function<void, Args...>::DerivedFunctor<RealRet, Obj, RealObj>* Function<void, Args...>::DerivedFunctor<RealRet, Obj, RealObj>::Clone() const
    {
        return new DerivedFunctor(*this);
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    void Function<void, Args...>::DerivedFunctor<RealRet, Obj, RealObj>::Execute(Args ... args) const
    {
        (obj->*func)(std::forward<Args>(args)...);
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    typename Function<void, Args...>::Type Function<void, Args...>::DerivedFunctor<RealRet, Obj, RealObj>::GetType() const
    {
        return Type::FUNCTOR;
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    bool Function<void, Args...>::DerivedFunctor<RealRet, Obj, RealObj>::Compare(const Function& arg) const
    {
        DerivedFunctor* convertedArg(static_cast<DerivedFunctor*>(arg.base.get()));
        return func == convertedArg->func && obj == convertedArg->obj;
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    Function<void, Args...>::DerivedBinderFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Impl(Wrapped func, RealObj* obj, Binder<BinderArgs...>&& bind) : DerivedFunctorBase(obj), func(func), boundArgs(std::move(bind.tuple))
    {}

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    Function<void, Args...>::DerivedBinderFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>* Function<void, Args...>::DerivedBinderFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Clone() const
    {
        return new Impl(*this);
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    void Function<void, Args...>::DerivedBinderFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Execute(Args ... args) const
    {
        BinderUnfolder<sizeof...(BinderArgs), Wrapped, BoundTupleT>::CheckPlaceholders<0>();
        BinderUnfolder<sizeof...(BinderArgs), Wrapped, BoundTupleT>::Do(func, obj, boundArgs, std::tuple<Args...>(std::forward<Args>(args)...));
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    typename Function<void, Args...>::Type Function<void, Args...>::DerivedBinderFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::GetType() const
    {
        return Type::FUNCTOR;
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    bool Function<void, Args...>::DerivedBinderFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Compare(const Function& arg) const
    {
        Impl* convertedArg(static_cast<Impl*>(arg.base.get()));
        return func == convertedArg->func && obj == convertedArg->obj && boundArgs == convertedArg->boundArgs;
    }

    template<class... Args>
    template<class RealObj>
    Function<void, Args...>::DerivedConstFunctorBase<RealObj>::DerivedConstFunctorBase(const RealObj* obj) : obj(obj)
    {}

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    Function<void, Args...>::DerivedConstFunctor<RealRet, Obj, RealObj>::DerivedConstFunctor(Wrapped func, const RealObj* obj) : DerivedConstFunctorBase(obj), func(func)
    {}

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    typename Function<void, Args...>::DerivedConstFunctor<RealRet, Obj, RealObj>* Function<void, Args...>::DerivedConstFunctor<RealRet, Obj, RealObj>::Clone() const
    {
        return new DerivedConstFunctor(*this);
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    void Function<void, Args...>::DerivedConstFunctor<RealRet, Obj, RealObj>::Execute(Args ... args) const
    {
        (obj->*func)(std::forward<Args>(args)...);
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    typename Function<void, Args...>::Type Function<void, Args...>::DerivedConstFunctor<RealRet, Obj, RealObj>::GetType() const
    {
        return Type::CONST_FUNCTOR;
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj>
    bool Function<void, Args...>::DerivedConstFunctor<RealRet, Obj, RealObj>::Compare(const Function& arg) const
    {
        DerivedConstFunctor* convertedArg(static_cast<DerivedConstFunctor*>(arg.base.get()));
        return func == convertedArg->func && obj == convertedArg->obj;
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    Function<void, Args...>::DerivedBinderConstFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Impl(Wrapped func, const RealObj* obj, Binder<BinderArgs...>&& bind) : DerivedConstFunctorBase(obj), func(func), boundArgs(std::move(bind.tuple))
    {}

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    Function<void, Args...>::DerivedBinderConstFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>* Function<void, Args...>::DerivedBinderConstFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Clone() const
    {
        return new Impl(*this);
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    void Function<void, Args...>::DerivedBinderConstFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Execute(Args ... args) const
    {
        BinderUnfolder<sizeof...(BinderArgs), Wrapped, BoundTupleT>::CheckPlaceholders<0>();
        BinderUnfolder<sizeof...(BinderArgs), Wrapped, BoundTupleT>::Do(func, obj, boundArgs, std::tuple<Args...>(std::forward<Args>(args)...));
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    typename Function<void, Args...>::Type Function<void, Args...>::DerivedBinderConstFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::GetType() const
    {
        return Type::CONST_FUNCTOR;
    }

    template<class... Args>
    template<class RealRet, class Obj, class RealObj, class... RealArgs>
    template<class... BinderArgs>
    bool Function<void, Args...>::DerivedBinderConstFunctor<RealRet, Obj, RealObj, RealArgs...>::Impl<BinderArgs...>::Compare(const Function& arg) const
    {
        Impl* convertedArg(static_cast<Impl*>(arg.base.get()));
        return func == convertedArg->func && obj == convertedArg->obj && boundArgs == convertedArg->boundArgs;
    }

    template<class Ret, class... Args>
    struct FunctionTraits<Function<Ret, Args...>>
    {
        typedef Function<Ret, Args...> FunctionT;

        typedef Ret ReturnT;
        typedef ParameterPack<Args...> ParameterPackT;
        static constexpr ParameterIndex parameterCount = ParameterPackT::count;

        template<ParameterIndex index>
        struct Parameter
        {
            typedef typename DiscoverType<index, Args...>::Type Type;
        };
    };

    template<class Ret, class... Args>
    Function<Ret, Args...> CreateFunction(Ret(*func)(Args...))
    {
        return Function<Ret, Args...>(func);
    }

    template<class Ret, class Obj, class RealObj, class... Args>
    Function<Ret, Args...> CreateFunction(Ret(Obj::*func)(Args...), RealObj* obj)
    {
        return Function<Ret, Args...>(func, obj);
    }

    template<class Ret, class Obj, class RealObj, class... Args>
    Function<Ret, Args...> CreateFunction(Ret(Obj::*func)(Args...), RealObj& obj)
    {
        return Function<Ret, Args...>(func, obj);
    }

    template<class Ret, class Obj, class RealObj, class... Args>
    Function<Ret, Args...> CreateFunction(Ret(Obj::*func)(Args...) const, const RealObj* obj)
    {
        return Function<Ret, Args...>(func, obj);
    }

    template<class Ret, class Obj, class RealObj, class... Args, typename std::enable_if<!std::is_pointer<RealObj>::value, int>::type = 0>
    Function<Ret, Args...> CreateFunction(Ret(Obj::*func)(Args...) const, const RealObj& obj)
    {
        return Function<Ret, Args...>(func, obj);
    }
}