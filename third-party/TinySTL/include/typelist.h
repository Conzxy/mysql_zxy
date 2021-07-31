////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//typelist is a type container as a component of type metaprogramming
//Construction:
//1. Typelist</Types.../>							(general)
//2. Cons<T1,Cons<T2,Cons<T3,...,Cons<TN>...>>		(Cons cell)
//3. Valuelist<Type,Values...>						(valuelist,the first parameter is a type,and then there are some values)

//The provided algorithm as following:
//1. Push_Front
//2. Pop_Front
//3. Push_Back
//4. Pop_Back
//5. Front
//6. Back
//7. Length
//8. Is_Empty
//9. Type_At
//10.Reverse
//11.Transform
//12.Accumulate
//13.Sort
//14.Erase
//15.Erase_All
//16.Replace
//17.Replace_All
//18.Index_Of
//19.Unique
//20.Largest_Type(also can be implemented by the Accumulate algorithm)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TYPELIST_H
#define TYPELIST_H

#include <cstdint>
#include "stl_type_traits_base.h"

namespace TinySTL {
	namespace mpl {

		namespace detail {
			using int_=std::intmax_t;
		}

		using int_=detail::int_;

///////////////////////////////////////////////////////////
// class template Typelist
// provide typelist container
///////////////////////////////////////////////////////////
		template<typename ...Elements>
		struct Typelist {};

///////////////////////////////////////////////////////////////
// class template Valuelist
// provide a typelist with sequence of compile-time values
///////////////////////////////////////////////////////////////
		template<typename T, T... Values>
		struct Valuelist {};


//////////////////////////////////////////////////////////////////////////////
// class template CValue
// helper to wrap a compile-time value as a type(that is,type wrapper)
//////////////////////////////////////////////////////////////////////////////
		template<typename T, T Value>
		struct CValueT {
			static constexpr T value=Value;
		};

/////////////////////////////////////////////		
// class template Nil
// represents a empty list
/////////////////////////////////////////////
		struct Nil {};


//////////////////////////////////////////////////
// class template Cons
// represents a Cons cell,contains two componet
// which hold two types
//////////////////////////////////////////////////
		template<typename HeadT, typename TailT=Nil>
		struct Cons {
			using Head=HeadT;
			using Tail=TailT;
		};

		template<typename Type>
		struct Type_identity {
			using type=Type;
		};


//type list algorithm
		namespace TL {

///////////////////////////////////////////////////////////
// class template Select
// reuse the TinySTL::Conditional
// because TinySTL::Is_function has used the typelist
// Invocation(Cond is a predicate ,T and F are types):
// Select<Cond,T,F>
///////////////////////////////////////////////////////////
	
			template<bool Cond>
			struct Select_{
				template<typename T, typename F>
				using type = T;
			};
	
			template<>
			struct Select_<false>{
				template<typename T, typename F>
				using type = F;
			};
			
			template<bool Cond, typename T, typename F>
			using Select = typename Select_<Cond>::template type<T, F>;

//////////////////////////////////////////////////////////
// class template Length
// obtains the number of elements of a typelist
// Invocation(List is a typelist):
// Length<List>
//////////////////////////////////////////////////////////

			namespace detail {
				template<typename List>
				struct Length_ {
					static constexpr int_ value=0;
				};

				template<typename ...Elems>
				struct Length_<Typelist<Elems...>> {
					static constexpr int_ value=sizeof...(Elems);
				};

				template<typename T, T ...Values>
				struct Length_<Valuelist<T, Values...>> {
					static constexpr int_ value=sizeof...(Values);
				};

				template<typename Head, typename Tail>
				struct Length_<Cons<Head, Tail>> {
					static constexpr int_ value=1+Length_<Tail>::value;
				};

				template<>
				struct Length_<Nil> {
					static constexpr int_ value=0;
				};

				
			}
			
			template<typename List>
			using Length = detail::Length_<List>;

//////////////////////////////////////////////////////////
// class template Is_Empty
// get the Boolean value according to the typelist if empty
// Invocation(List is a typelist):
// Is_Empty<List>
//////////////////////////////////////////////////////////

			namespace detail {
				template<typename List>
				struct Is_Empty_ :_false_type {
				};

				template<>
				struct Is_Empty_<Typelist<>> :_true_type {
				};

				template<typename T>
				struct Is_Empty_<Valuelist<T>> :_true_type {
				};

				template<>
				struct Is_Empty_<Nil> :_true_type {};
			}

			template<typename List>
			using Is_Empty = detail::Is_Empty_<List>;

/////////////////////////////////////////////////////////
// class template Front
// obtains the first element(type) of typelist
// Invocation(List is a typelist):
// Front<List> or FrontT<List>::type
//////////////////////////////////////////////////////////
			namespace detail {

//primary template for cons cell
				template<typename List>
				struct Front_
					:Type_identity<typename List::Head> {};

				template<typename Head, typename ...Tail>
				struct Front_<Typelist<Head, Tail...>> {
					using type=Head;
				};

				template<typename T, T Head, T ...Values>
				struct Front_<Valuelist<T, Head, Values...>> {
					using type=CValueT<T, Head>;
				};
			}

			template<typename List>
			using FrontT=detail::Front_<List>;

			template<typename List>
			using Front=typename detail::Front_<List>::type;

////////////////////////////////////////
// class template Pop_Front
// pop the first element of Typelist
// Invocation(List is a typelist):
// Pop_Front<List> or Pop_FrontT<List>::type
////////////////////////////////////////
			namespace detail {

				//primary template for Cons cell				
				template<typename List>
				struct Pop_Front_
					:Type_identity<typename List::Tail> {};

				template<typename Head, typename ...Tail>
				struct Pop_Front_<Typelist<Head, Tail...>> {
					using type=Typelist<Tail...>;
				};

				template<typename T, T value, T ...Values>
				struct Pop_Front_<Valuelist<T, value, Values...>> {
					using type=Valuelist<T, Values...>;
				};

				
			}

			template<typename List>
			using Pop_FrontT=detail::Pop_Front_<List>;

			template<typename List>
			using Pop_Front=typename detail::Pop_Front_<List>::type;

////////////////////////////////////////////////////////////////////
// class template Push_Front
// appends the given type to the firts position of typelist
// Invocation(List is a typelist and NewElement is a type):
// Push_Front<List,NewElement> or Push_FrontT<List,NewElement>::type
////////////////////////////////////////////////////////////////////

			namespace detail {
//primary template for Cons cell
				template<typename List, typename New>
				struct Push_Front_
					:Type_identity<Cons<New, List>> {};

				template<typename ...Elem, typename New>
				struct Push_Front_<Typelist<Elem...>, New> {
					using type=Typelist<New, Elem...>;
				};

				template<typename T, T... Values, T value>
				struct Push_Front_<Valuelist<T, Values...>, CValueT<T, value>> {
					using type=Valuelist<T, value, Values...>;
				};
			}

			template<typename List, typename T>
			using Push_FrontT=detail::Push_Front_<List, T>;

			template<typename List, typename T>
			using Push_Front=typename detail::Push_Front_<List, T>::type;

//////////////////////////////////////////////////////////////////
// class template Push_Back
// appends the new element to the back of the typelist
// Invocation(List is a typelist and NewElem is a type):
// Push_Back<List,NewElem> or Push_BackT<List,NewElem>::type
//////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename List, typename New,
						 bool=Is_Empty<List>::value>
				struct Push_Back_Rec;

				template<typename List, typename New>
				struct Push_Back_Rec<List, New, false> {
				private:
					using Head=Front<List>;
					using Tail=Pop_Front<List>;
					using NewTail=typename Push_Back_Rec<Tail, New>::type;
				public:
					using type=Push_Front<NewTail, Head>;
				};

				template<typename List, typename New>
				struct Push_Back_Rec<List, New, true> {
					using type=Push_Front<List, New>;
				};

				template<typename List, typename T>
				struct Push_Back_
					:Push_Back_Rec<List, T> {};

				template<typename ...Elems, typename New>
				struct Push_Back_<Typelist<Elems...>, New> {
					using type=Typelist<Elems..., New>;
				};

				template<typename T, T value, T... Values>
				struct Push_Back_<Valuelist<T, Values...>, CValueT<T, value>> {
					using type=Valuelist<T, Values..., value>;
				};

			}

			template<typename List, typename T>
			using Push_BackT=detail::Push_Back_<List, T>;

			template<typename List, typename T>
			using Push_Back=typename detail::Push_Back_<List, T>::type;


////////////////////////////////////////////////////////////////////
// class template Reverse
// reverse the typelist
// Invocation(List is a typelist):
// Reverse<List> or ReverseT<List>::type
////////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename List, bool=Is_Empty<List>::value>
				struct Reverse_;

				/*template<typename List>
				struct Reverse_<List, false> {
					using Head=Front<List>;
					using type=Push_Back<typename Reverse_<Pop_Front<List>>::type, Head>;
				};*/

				template<typename List>
				struct Reverse_<List, false>
					:Push_BackT<typename Reverse_<Pop_Front<List>>::type, Front<List>> {};

				template<typename List>
				struct Reverse_<List, true> {
					using type=List;
				};
			}

			template<typename List>
			using ReverseT=detail::Reverse_<List>;

			template<typename List>
			using Reverse=typename detail::Reverse_<List>::type;

//////////////////////////////////////////////////////////////////
// class template Back
// obtains the last element of a typelist
// Invocation(List is a typelist):
// Back<List> or BackT<List>::type
//////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename List>
				struct Back_
					:Type_identity<Front<Reverse<List>>> {};
			}

			template<typename List>
			using BackT=detail::Back_<List>;

			template<typename List>
			using Back=typename detail::Back_<List>::type;

//////////////////////////////////////////////////////////////////////
// class template Pop_Back
// removes the last element of a typelist
// Invocation(List is a typelist):
// Pop_Back<List> or Pop_BackT<List>::type
//////////////////////////////////////////////////////////////////////
			namespace detail {
				template<typename List>
				struct Pop_Back_
					:Type_identity<Reverse<Pop_Front<Reverse<List>>>> {};

			}

			template<typename List>
			using Pop_BackT=detail::Pop_Back_<List>;

			template<typename List>
			using Pop_Back=typename detail::Pop_Back_<List>::type;

/////////////////////////////////////////////////////////////////////////////
// class template Type_At
// obtains the type at a given index in a typelist
// Invocation(List is a typelist and N is a compile-time integral value)
// Type_At<List,N> or Type_AtT<List,N>::type
/////////////////////////////////////////////////////////////////////////////
			namespace detail {

				template<typename List, int_ N>
				struct Type_At_ 
					:Type_At_<Pop_Front<List>,N-1>{
					static_assert(!Is_Empty<List>::value, "Typelist is empty!");
				};

				template<typename List>
				struct Type_At_<List, 0>
					:Type_identity<Front<List>> {};

			}

			template<typename List, int_ N>
			using Type_AtT=detail::Type_At_<List, N>;

			template<typename List, int_ N>
			using Type_At=typename detail::Type_At_<List, N>::type;

///////////////////////////////////////////////////////////
// class template Index_Of
// Finds the index of a given type in a typelist
// Invovation(TL is a typelist type and T is a type):
// Index_Of<TL,T>
///////////////////////////////////////////////////////////

			namespace detail {
				template<typename TL, typename T,
						bool=Is_Empty<TL>::value>
				struct Index_Of_ {
					static constexpr int_ temp=Index_Of_<Pop_Front<TL>, T>::value;
					static constexpr int_ value=(temp==-1?-1:1+temp);
				};

				template<typename TL>
				struct Index_Of_ <TL, Front<TL>, false> {
					static constexpr int_ value=0;
				};

				template<typename TL, typename T>
				struct Index_Of_<TL, T, true> {
					static constexpr int_ value=-1;
				};

			}

			template<typename TL, typename T>
			using Index_Of = detail::Index_Of_<TL, T>;

///////////////////////////////////////////////////////////////////
// class template Erase
// erases the first occurrence
// Invocation(TL is a typelist type and T is a type):
// EraseT<TL,T>::type or Erase<TL,T>
///////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename TL, typename T,
						 bool=Is_Empty<TL>::value>
				struct Erase_
					:Push_FrontT<typename Erase_<Pop_Front<TL>, T>::type, Front<TL>> {};

				template<typename TL>
				struct Erase_<TL, Front<TL>, false>
					:Pop_FrontT<TL> {};

				template<typename TL, typename T>
				struct Erase_<TL, T, true>
					:Type_identity<TL> {};
			}

			template<typename TL, typename T>
			using EraseT=detail::Erase_<TL, T>;

			template<typename TL, typename T>
			using Erase=typename detail::Erase_<TL, T>::type;

//////////////////////////////////////////////////////////////////
// Class template Erase_All
// erases all first occurrences
// Invocation(TL is a typelist and T is a type):
// Erase_AllT<TL,T>::type or Erase_All<TL,T>
//////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename TL, typename T,
					     bool=Is_Empty<TL>::value>
				struct Erase_All_
					:Push_FrontT<typename Erase_All_<Pop_Front<TL>, T>::type, Front<TL>> {};

				template<typename TL>
				struct Erase_All_<TL, Front<TL>, false>
					:Erase_All_<Pop_Front<TL>, Front<TL>> {};

				template<typename TL, typename T>
				struct Erase_All_<TL, T, true>
					:Type_identity<TL> {};
			}

			template<typename TL, typename T>
			using Erase_AllT=detail::Erase_All_<TL, T>;

			template<typename TL, typename T>
			using Erase_All=typename detail::Erase_All_<TL, T>::type;

///////////////////////////////////////////////////////////////
// class template Unique
// Removes all duplicate types in a typelist,only reserve the first occurrence
// Invocation(TL is a typelist):
// UniqueT<TL>::type or Unique<TL>
///////////////////////////////////////////////////////////////

			namespace detail {
				template<typename TL, bool=Is_Empty<TL>::value>
				struct Unique_;

				template<typename TL>
				struct Unique_<TL, false> {
					using Head=Front<TL>;
					using Tail=typename Unique_<Pop_Front<TL>>::type;

					using type=Push_Front<Erase<Tail, Head>, Head>;
				};

				template<typename TL>
				struct Unique_<TL, true>
					:Type_identity<TL> {};
			}

			template<typename TL>
			using UniqueT=detail::Unique_<TL>;

			template<typename TL>
			using Unique=typename detail::Unique_<TL>::type;

//////////////////////////////////////////////////////////////////////////////
// class template Replace
// Replaces the first occurrence of a type in a typelist,with another type
// Invocation(TL is a typelist type ,cur is the occurrence type 
//			  and Rp is the replacement type):
// Replace<TL,cur,Rp> or ReplaceT<TL,cur,Rp>::type
//////////////////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename TL, typename Cur, typename Rp,
						 bool=Is_Empty<TL>::value>
				struct Replace_
					:Push_FrontT<typename Replace_<Pop_Front<TL>, Cur, Rp>::type, Front<TL>> {};

				template<typename TL, typename Rp>
				struct Replace_<TL, Front<TL>, Rp, false>
					:Push_FrontT<Pop_Front<TL>, Rp> {};

				template<typename TL, typename Cur, typename Rp>
				struct Replace_<TL, Cur, Rp, true>
					:Type_identity<TL> {};
			}

			template<typename TL, typename Cur, typename Rp>
			using ReplaceT=detail::Replace_<TL, Cur, Rp>;

			template<typename TL, typename Cur, typename Rp>
			using Replace=typename detail::Replace_<TL, Cur, Rp>::type;

//////////////////////////////////////////////////////////////////////
// class template Replace_All
// replace all occurrent of a type in a typelist,with another type
// Invocation(TL is a typelist type ,cur is the occurrence type 
//			  and Rp is the replacement type):
// Replace_AllT<TL,cur,Rp>::type or Replace_All<TL,cur,Rp>
//////////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename TL, typename Cur, typename Rp,
						bool=Is_Empty<TL>::value>
				struct Replace_All_
					:Push_FrontT<typename Replace_All_<Pop_Front<TL>, Cur, Rp>::type, Front<TL>> {};

				template<typename TL, typename Rp>
				struct Replace_All_<TL, Front<TL>, Rp, false>
					:Replace_All_<Push_Front<Pop_Front<TL>, Rp>, Front<TL>, Rp> {};

				template<typename TL, typename Cur, typename Rp>
				struct Replace_All_<TL, Cur, Rp, true>
					:Type_identity<TL> {};
			}

			template<typename TL, typename Cur, typename Rp>
			using Replace_AllT=detail::Replace_All_<TL, Cur, Rp>;

			template<typename TL, typename Cur, typename Rp>
			using Replace_All=typename detail::Replace_All_<TL, Cur, Rp>::type;

			////////////////////////////////////////////////////
			// class template Largest_Type
			// finds the largest type within a typelist
			// Invocation(List is a type list):
			// Largest_Type<List> or Largest_TypeT<List>::type
			////////////////////////////////////////////////////
			namespace detail {
				template<typename List, bool=Is_Empty<List>::value>
				struct Largest_Type_;

				template<typename List>
				struct Largest_Type_<List, false> {
					using Head=Front<List>;
					using Rest=typename Largest_Type_<Pop_Front<List>>::type;

					using type=Select<(sizeof(Head)<sizeof(Rest)), Rest, Head>;
				};

				template<typename List>
				struct Largest_Type_<List, true> {
					using type=char;
				};
			}

			template<typename List>
			using Largest_TypeT=detail::Largest_Type_<List>;

			template<typename List>
			using Largest_Type=typename detail::Largest_Type_<List>::type;

////////////////////////////////////////////////////////////////////
// class template Transform
// takes a typelist and a metafunction and 
// produces another typelist containing the result
// of applying the metafunction to each type
// Invocation(List is a typelist and MetaFun is a metafunction):
// Transform<List,MetaFun> or TransformT<List,MetaFun>::type
////////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename List, template<typename > class MetaFun,
					bool=Is_Empty<List>::value>
					struct Transform_;

//generic version:fit for Cons cell	
				/*template<typename List,template<typename > class MetaFun>
				struct Transform_<List, MetaFun, false> {
					using Head=Front<List>;
					using Rest=typename Transform_<Pop_Front<List>,MetaFun>::type;
					using type=Push_Front<Rest, typename MetaFun<Head>::type>;

				};*/

				template<typename List, template<typename > class MetaFun>
				struct Transform_<List, MetaFun, false>
					:Push_Front<typename Transform_<Pop_Front<List>, MetaFun>::type,
					MetaFun<Front<List>>> {};

//rely on the Pack Expansions
				template<typename ...Elems, template<typename > class MetaFun>
				struct Transform_<Typelist<Elems...>, MetaFun, false> {
					using type=Typelist<typename MetaFun<Elems>::type...>;
				};

				template<typename List, template<typename > class MetaFun>
				struct Transform_<List, MetaFun, true> {
					using type=List;
				};
			}

			template<typename List, template<typename > class MetaFun>
			using TrasformT=detail::Transform_<List, MetaFun>;

			template<typename List, template<typename > class MetaFun>
			using Transform=typename detail::Transform_<List, MetaFun>::type;

/////////////////////////////////////////////////////////////////////////////
// class template Accumulate
// produces a single result by using metafunction to compute two types in turn 
// Invocation(List is a typelist,MetaFun is a metafunction,
//			  and Init is a initial type):
// Accumulate<List,MetaFun,Init> or AccumulateT<List,MetaFun,Init>::type
/////////////////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename List, template<typename, typename > class MetaFun, typename I,
						bool=Is_Empty<List>::value>
				struct Accumulate_;


//if EN=null type,return the result of F(F(...F(T1,I),T2),...,TN-1),TN)
//Note:the former ellipsis indicates F(,the latter indicates T)

				/*template<typename List,template<typename ,typename > class MetaFun,typename I>
				struct Accumulate_<List, MetaFun, I,false> {
					using Head=Front<List>;
					using Rest=Pop_Front<List>;
					using type=typename Accumulate_<Rest,MetaFun,typename MetaFun<Head, I>::type>::type;
				};*/

				//metafunction forwarding
				template<typename List, template<typename, typename > class MetaFun, typename I>
				struct Accumulate_<List, MetaFun, I, false>
					:Accumulate_<Pop_Front<List>, MetaFun,
					typename MetaFun<Front<List>, I>::type> {};

				template<typename List, template<typename, typename > class MetaFun, typename I>
				struct Accumulate_<List, MetaFun, I, true>
					:Type_identity<I> {};
			}

			template<typename List, template<typename, typename > class MetaFun, typename I>
			using AccumulateT=detail::Accumulate_<List, MetaFun, I>;

			template<typename List, template<typename, typename > class MetaFun, typename I>
			using Accumulate=typename detail::Accumulate_<List, MetaFun, I>::type;

////////////////////////////////////////////////////////////////////////////////////
// class template Sort
// sort a typelist through insertion Based on a Comparison
// Invocation(List is a typelist and Comp is a predicate for comparing elements):
// Sort<List,Comp> or SortT<List,Comp>::type
////////////////////////////////////////////////////////////////////////////////////

			namespace detail {

				template<typename List, template<typename, typename > class Comp,
						 bool=Is_Empty<List>::value>
				struct Sort_;

				template<typename List, template<typename, typename > class Comp>
				using Sort_t=typename Sort_<List, Comp>::type;


//insert a type into an already-sorted list at the first point that will keep the list sorted				
				template<typename List, typename Elem, template<typename, typename > class Comp,
						bool=Is_Empty<List>::value>
				struct Sorted_;

				template<typename List, template<typename, typename > class Comp>
				struct Sort_<List, Comp, false>
					: Sorted_<Sort_t<Pop_Front<List>, Comp>,
					Front<List>, Comp> {};

				template<typename List, template<typename, typename > class Comp>
				struct Sort_<List, Comp, true> {
					using type=List;
				};

				template<typename List, typename Elem, template<typename, typename > class Comp>
				struct Sorted_<List, Elem, Comp, false> {
				private:
					using NewTail=typename Select<
													Comp<
														 Elem,
														 Front<List>
													>::value,
													Type_identity<List>,
													Sorted_<Pop_Front<List>, Elem, Comp>
					>::type;

					using NewHead=Select<
										    Comp<
											     Elem,
												 Front<List>
											>::value,
											Elem,
											Front<List>
					>;

				public:
					using type=Push_Front<NewTail, NewHead>;
				};

				/*template<typename List,typename Elem,template<typename ,typename> class Comp>
				struct Sorted_<List, Elem,Comp, false> {
					using type=Select<Comp<Elem, Front<List>>::value,
						Push_Front<List, Elem>,
						Push_Front<typename Sorted_<Pop_Front<List>, Elem, Comp>::type, Front<List>>>;
				};*/

				template<typename List, typename Elem, template<typename, typename > class Comp>
				struct Sorted_<List, Elem, Comp, true>
					:Push_FrontT<List, Elem> {};

			}
			template<typename List, template<typename, typename > class Comp>
			using SortT=detail::Sort_<List, Comp>;

			template<typename List, template<typename, typename > class Comp>
			using Sort=typename detail::Sort_<List, Comp>::type;

/////////////////////////////////////////////////////////////////////
// class template Pick
// select the elements in a given list of indices to produce
// a new typelist
// Invocation(TL is a typelist type and Indices is 
// Pick<TL,Indices> or PickT<TL,Indices>::type
/////////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename TL, typename Indices>
				struct Pick_ {
				};

				template<typename TL, int_... Vs>
				struct Pick_<TL, Valuelist<int_, Vs...>> {
					using type=Typelist<Type_At<TL, Vs>...>;
				};
			}

			template<typename TL, typename Indices>
			using PickT=detail::Pick_<TL, Indices>;

			template<typename TL, typename Indices>
			using Pick=typename detail::Pick_<TL, Indices>::type;
			
			// template vriable since c++14
#if __cplusplus >= 201402L
			template<typename List>
			constexpr auto Length_v = Length<List>::value;

			template<typename T>
			constexpr bool Is_Empty_v = Is_Empty<T>::value;

			template<typename TL, typename T>
			constexpr auto Index_Of_v = Index_Of<TL, T>::value;
#endif
		}	//namespace TL
	}	//namespace mpl
}	//namespace TinySTL

#endif //TYPELIST_H
