//tuple:the "struct" in TMP
//two strengths:
//* positional interface
//* can be constructed easily from a typelist
#ifndef TINYSTL_TUPLE_H
#define TINYSTL_TUPLE_H

#include "stl_move.h"
#include "config.h"
#if __cplusplus >= 201402L
#include <type_traits> // std::is_final
#endif
#include "typelist.h"
#include "utility.h"

namespace TinySTL{
    template<typename ...>
    class Tuple;

    /*template<typename Head,typename... Tail>
    class Tuple<Head,Tail...>{
    private:
        Head head;
        Tuple<Tail...> tail;

    public:
        //constructor
        Tuple(){}
        //Enable_if allow type conversion
        template<typename _Head,typename... _Tail,
                typename =STL_ Enable_if_t<sizeof...(Tail)==sizeof...(_Tail)>>
        Tuple(_Head&& _head,_Tail&&... _tail)
        :head{STL_ forward<_Head>(_head)},tail{STL_ forward<_Tail>(_tail)...}{}

        template<typename _Head,typename ..._Tail,
                typename =STL_ Enable_if_t<sizeof...(Tail)==sizeof...(_Tail)>>
        Tuple(Tuple<_Head,_Tail...> const& other)
        :head(other.getHead()),tail(other.getTail()){}

        Tuple(Head const& _Head,Tuple<Tail...> const& _Tail)
        :head(_Head),tail(_Tail){}

        //interface:get head/tail
        Head& getHead(){return head;}
        Head const& getHead()const{return head;}
        Tuple<Tail...>& getTail(){return tail;}
        Tuple<Tail...> const& getTail()const{return tail;}
    };*/

    template<int_ height, typename T, bool = 
            TinySTL::Is_class<T>::value
#if __cplusplus >= 201402L
				&& ! std::is_final<T>::value>
#else
			>
#endif
    class TupleElt;

    template<int_ height,typename T>
    class TupleElt<height,T,true>: private T{
    public:
        TupleElt()=default;

		template<typename U>
		TupleElt(U const& other)
			: T(other)
		{ }

        template<typename U>
        TupleElt(U&& other)
			: T(STL_ forward<U>(other))
		{ }

        T& get(){return *this;}
        T const& get()const{return *this;}
    };

    template<int_ height,typename T>
    class TupleElt<height,T,false>{
    private:
        T value;
    public:
        TupleElt()=default;
	
		template<typename U>
		TupleElt(U const& val)
			: value(val)
		{ }

        template<typename U>
        TupleElt(U&& val)
			: value(STL_ forward<U>(val))
		{ }

        T& get(){return value;}
        T const& get()const{return value;}
    };

    template<typename Head,typename ...Tail>
    class Tuple<Head,Tail...>
            :private TupleElt<sizeof...(Tail),Head>,private Tuple<Tail...>{
        using HeadElt=TupleElt<sizeof...(Tail),Head>;
    public:
        Tuple()=default;
        template<typename _Head,typename ..._Tail,
                typename =STL_ Enable_if_t<sizeof...(_Tail)==sizeof...(Tail)>>
        Tuple(_Head&& head,_Tail&&... tail)
                :HeadElt(STL_ forward<_Head>(head)),
                Tuple<Tail...>(STL_ forward<_Tail>(tail)...){}

        template<typename _Head,typename ..._Tail,
                typename =STL_ Enable_if_t<sizeof...(Tail)==sizeof...(_Tail)>>
        Tuple(Tuple<_Head,_Tail...> const& other)
                :HeadElt(other.getHead()),Tuple<Tail...>(other.getTail()){}

        template<typename _Head,typename ..._Tail,
                typename =STL_ Enable_if_t<sizeof...(Tail)==sizeof...(_Tail)>>
        Tuple(_Head const& head,Tuple<_Tail...> const& tail)
                :HeadElt(head),Tuple<Tail...>(tail){}
		
		
		template<typename ...Args, typename = Enable_if_t<
			sizeof...(Args) == sizeof...(Tail) + 1
		>>
		Tuple(Tuple<Args...> const& other)
			: HeadElt(other.getHead())
			, Tuple<Tail...>(other.getTail())
		{ }

		template<typename ...Args, typename = Enable_if_t<
			sizeof...(Args) == sizeof...(Tail) + 1
		>>
		Tuple(Tuple<Args...>&& other) noexcept
			: HeadElt(STL_MOVE(other.getHead()))
			, Tuple<Tail...>(STL_MOVE(other.getTail()))
		{ }
		
		template<typename ...Args, typename = Enable_if_t<
			sizeof...(Args) == sizeof...(Tail) + 1
		>>
		Tuple& operator=(Tuple<Args...> const& other){
			auto tmp = other;
			this->swap(other);
			return *this;
		}

		template<typename ...Args, typename = Enable_if_t<
			sizeof...(Args) == sizeof...(Tail) + 1
		>>
		Tuple& operator=(Tuple<Args...>&& other) noexcept {
			this->swap(other);

			return *this;
		}

		void swap(Tuple& rhs) noexcept{
			STL_SWAP(getHead(), rhs.getHead());
			STL_SWAP(getTail(), rhs.getTail());
		}

        Head& getHead(){
            return static_cast<HeadElt*>(this)->get();
        }

        Head const& getHead()const{
            return static_cast<HeadElt const*>(this)->get();
        }

        Tuple<Tail...>& getTail(){
            return *this;
        }

        Tuple<Tail...> const& getTail()const{
            return *this;
        }
    };

    template<>
    class Tuple<>{};

    //make_Tuple(elems...)
    //obtains a new Tuple object for given elements

    //note:decay remove the cv qualifier and references
    //convert built-in array type to pointer type
    //convert function type to pointer-to-function type
    template<typename ...Types>
    auto make_Tuple(Types&&... elems)
		-> Tuple<STL_ Decay_t<Types>...>{
        return Tuple<STL_ Decay_t<Types>...>(STL_ forward<Types>(elems)...);
    }

    //obtains the element at a given index in a tuple
    //Get<index>(tuple-object)
    template<int_ N>
    struct TupleGet{
        template<typename ...Types>
        inline static auto apply(Tuple<Types...>& self)
		-> decltype(TupleGet<N-1>::apply(self.getTail())){
            return TupleGet<N-1>::apply(self.getTail());
        }
    };

    template<>
    struct TupleGet<0>{
        template<typename ...Types>
        inline static auto apply(Tuple<Types...>& self)
			->decltype(self.getHead()){
            return self.getHead();
        }
    };

    template<int_ N,typename ...Types>
    auto Get(Tuple<Types...>& self)
		-> decltype(TupleGet<N>::apply(self)){
        return TupleGet<N>::apply(self);
    }

    //Comparison
    //Tuple_1==Tuple_2
    constexpr bool operator==(Tuple<> const&,Tuple<> const&){
        return true;
    }

    template<typename Head1,typename ...Tail1,
            typename Head2,typename ...Tail2,
            typename =STL_ Enable_if_t<sizeof...(Tail1)==sizeof...(Tail2)>>
    bool operator ==(Tuple<Head1,Tail1...> const& t1,Tuple<Head2,Tail2...> const& t2){
        /*if(t1.getHead()==t2.getHead())
            return t1.getTail()==t2.getTail();
        else
            return false;*/
        return t1.getHead()==t2.getHead()&&
            t1.getTail()==t2.getTail();
    }

    //fall back if sizeof...(Tail1)!=sizeof...(Tail2)
    template<typename ...Types1,typename ...Types2>
    bool operator ==(Tuple<Types1...> const&,Tuple<Types2...> const& ){
        return false;
    }

    //Output
	template<typename Ostream>
    void PrintTuple(Ostream& os,Tuple<> const&,bool isFirst=true){
        os<<(isFirst ? "()" : ")");
    }

    template<typename Ostream, typename ...Types>
    void PrintTuple(Ostream& os,Tuple<Types...> const& t,bool isFirst=true){
        os<<(isFirst ? "(" : ", ");
        os<<t.getHead();
        PrintTuple(os,t.getTail(),false);
    }

    template<typename Ostream, typename ...Types>
    Ostream& operator<<(Ostream&& os,Tuple<Types...> const& t){
        PrintTuple(os,t);
        return os;
    }

    namespace mpl{
            namespace TL{
                namespace detail{
                    template<>
                    struct Is_Empty_<Tuple<>>{
                        constexpr static bool value=true;
                    };

                    template<typename Head,typename ...Tail>
                    struct Front_<Tuple<Head,Tail...>>{
                        using type=Head;
                    };

                    template<typename ...Types,typename Elem>
                    struct Push_Front_<Tuple<Types...>,Elem>{
                        using type=Tuple<Types...,Elem>;
                    };

                    template<typename Head,typename ...Tail>
                    struct Pop_Front_<Tuple<Head,Tail...>>{
                        using type=Tuple<Tail...>;
                    };

                    template<typename ...Types,typename Elem>
                    struct Push_Back_<Tuple<Types...>,Elem>{
                        using type=Tuple<Types...,Elem>;
                    };
                }   //namespace detail
            }   //namespace TL
        }   //namespace mpl

        //Push_Front
        template<typename ...Types,typename New>
        auto Push_Front(Tuple<Types...> const& t,New const& value)
		-> mpl::TL::Push_Front<Tuple<Types...>, New>{
            return mpl::TL::Push_Front<Tuple<Types...>,New>(value,t);
        }

        //Push_Back
        template<typename Head,typename ...Tail,typename New>
        auto Push_Back(Tuple<Head,Tail...> const& t,New const& value)
		-> Tuple<Head, Tail..., New>{
            return Tuple<Head,Tail...,New>(t.getHead(),Push_Back(t.getTail(),value));
        }

        template<typename New>
        auto Push_Back(Tuple<> const&,New const& value)
			-> Tuple<New> {
            return Tuple<New>(value);
        }

        //Pop_Back
        template<typename Head,typename ...Tail>
        auto Pop_Front(Tuple<Head,Tail...> const& t)
			-> decltype(t.getTail()){
            return t.getTail();
        }

        //Pop_Back
        template<typename Head,typename ...Tail>
        auto Pop_Back(Tuple<Head,Tail...> const& t)
			-> decltype(Reverse(Pop_Front(Reverse(t)))){
            return Reverse(Pop_Front(Reverse(t)));
        }
        //Make_Indexlist
        //Invocation:Make_Indexlist<N>(N is integer value)
        //generates a index sequence in the form of Valuelist<integer_type,0,1,...,N-1>
        template<int_ N,typename Result=mpl::Valuelist<int_>>
        struct Make_IndexlistT
                :Make_IndexlistT<N-1,mpl::TL::Push_Front<Result,mpl::CValueT<int_,N-1>>>{};

        template<typename Result>
        struct Make_IndexlistT<0,Result>{
            using type=Result;
        };

        template<int_ N>
        using Make_Indexlist=typename Make_IndexlistT<N>::type;

        //Pick
        //core operation for many algorithm of Tuple
        //make a new tuple object according to a given index list of indices
        //Invocation(t is a tuple object,vl is a  Valuelist object):
        //Pick(t,vl)

        template<typename ...Elems,int_ ...Indices>
        auto Pick(Tuple<Elems...> const& t,mpl::Valuelist<int_,Indices...>)
			-> decltype(make_Tuple(Get<Indices>(t)...)){
            return make_Tuple(Get<Indices>(t)...);
        }

        //Reverse
        //In standard c++11,the return type has to be declared as tailing version:
        //->decltype(...)
        template<typename ...Elems>
        auto Reverse(Tuple<Elems...> const& t)
        -> decltype(Pick(t,mpl::TL::Reverse<Make_Indexlist<sizeof...(Elems)>>()))
        {
            return Pick(t,mpl::TL::Reverse<Make_Indexlist<sizeof...(Elems)>>());
        }

        //discard the reverse version as following
        //Since there are too many copy costs

        /*template<typename Head,typename ...Tail>
        auto Reverse(Tuple<Head,Tail...> const&t){
            return Push_Back(Reverse(t.getTail()),t.getHead());
        }

        Tuple<> Reverse(Tuple<> const&){
            return Tuple<>();
        }*/

        //Replicate
        //
        template<int_ I,int_ N,typename IndexList=mpl::Valuelist<int_>>
        struct ReplicateT;

        template<int_ I,int_ N,int_ ...Indices>
        struct ReplicateT<I,N,mpl::Valuelist<int_,Indices...>>
                :ReplicateT<I,N-1,mpl::Valuelist<int_,Indices...,I>>{};

        template<int_ I,int_ ...Indices>
        struct ReplicateT<I,0,mpl::Valuelist<int_,Indices...>>{
            using type=mpl::Valuelist<int_,Indices...>;
        };

        template<int_ I,int_ N>
        using Replicate=typename ReplicateT<I,N>::type;

        //splat
        //may be the abbr of "split at" Since the meaning of "splat" doesn't feel right
        //produce a "replicated" index set consisting of N copies of the value I(index)
        template<int_ I,int_ N,typename ...Elems>
        auto splat(Tuple<Elems...> const& t)
			-> decltype(Pick(t, Replicate<I, N>())){
            return Pick(t,Replicate<I,N>());
        }

        //metafunction warpper that compares the elements in a tuple
        template<typename List,template<typename ,typename > class TMetaFun>
        struct MetaFun_Of_NthElement
        {
            template<typename ,typename > struct apply;

            template<int_ N,int_ M>
            struct apply<mpl::CValueT<int_,N>,mpl::CValueT<int_,M>>
                    :TMetaFun<TL_ Type_At<List,N>,TL_ Type_At<List,M>>{};
        };

        template<template<typename ,typename > class Comp,typename ...Elems>
        auto sort(Tuple<Elems...> const& t)
			-> decltype(Pick(t,
                        TL_ Sort<Make_Indexlist<sizeof...(Elems)>,
                                MetaFun_Of_NthElement<Tuple<Elems...>,Comp>::template apply>()))
        {
            return Pick(t,
                        TL_ Sort<Make_Indexlist<sizeof...(Elems)>,
                                MetaFun_Of_NthElement<Tuple<Elems...>,Comp>::template apply>());
        }

		template<typename Ostream>
		Ostream& PrintValuelist(Ostream& os, mpl::Valuelist<int_>, bool is_first=true){
			os << (is_first ? "[]" : "]");
			return os;
		}

		template<typename Ostream, int_... values>
		Ostream& PrintValuelist(Ostream& os, mpl::Valuelist<int_, values...>, bool is_first = true) {
			using Cur = mpl::Valuelist<int_, values...>;
			os << ((is_first) ? "[" : ", ");
			os << mpl::TL::Front<Cur>::value;
			return PrintValuelist(os, mpl::TL::Pop_Front<Cur>{}, false);
		}


		template<typename Ostream, typename T, int_ ...indices>
		Ostream& operator<<(Ostream& os, mpl::Valuelist<T, indices...> x){
			return PrintValuelist(os, x);
		}

		template<typename F, typename ...Args, int_... indices>
		auto for_each_impl(Tuple<Args...>& tuple, F&& func, mpl::Valuelist<int_, indices...>)
		-> decltype(make_Tuple(func(Get<indices>(tuple))...)){
			return make_Tuple(func(Get<indices>(tuple))...);
		}

		template<typename F, typename ...Args>
		auto for_each(Tuple<Args...>& tuple, F&& func)
		-> decltype(for_each_impl(tuple, STL_FORWARD(F, func), 
					Make_Indexlist<sizeof...(Args)>{})){
			using Indices = Make_Indexlist<sizeof...(Args)>;
			

			return for_each_impl(tuple, STL_FORWARD(F, func), Indices{});
		}
}

#endif //TINYSTL_TUPLE_H
