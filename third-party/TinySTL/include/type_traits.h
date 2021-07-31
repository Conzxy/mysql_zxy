#ifndef TINYSTL_TYPE_TRAITS_H
#define TINYSTL_TYPE_TRAITS_H

//该文件实现了标准库type_traits（×），templates练习（√）
//其中需要借助于编译器的语义traits目前无法实现(e.g.  trivially's)
#include <cstddef>
#include <cstdint>
#include "stl_type_traits_base.h"
#include "typelist.h"

namespace TinySTL {
//check if a given type member exists
#define DEFINE_HAS_TYPE(Memtype)						        \
template<typename,typename =Void_t<>>				            \
struct HasTypeT_##Memtype							            \
:_false_type{};										            \
template<typename T>								            \
struct HasTypeT_##Memtype<T,Void_t<typename T::Memtype>>		\
:_true_type{};                                                  \

//check if a given member exists
#define DEFINE_HAS_MEMBER(Member)                               \
template<typename,typename =Void_t<>>                           \
struct HasMemberT_##Member                                      \
:_false_type{};                                                 \
template<typename T>                                            \
struct HasMemberT_##Member<T,Void_t<decltype(&T::Member)>>      \
:_true_type{};                                                  \

	/////////////////////////
	///////basic tool////////
	/////////////////////////
	//provide SFINAE context to SFINAE out failure version 
	template<typename...>
	using Void_t=void;

	//chooses one type or another based on compile-time Boolean 
	namespace detail {
		template<bool>
		struct Conditional_;
	}

	template<bool Cond, typename T, typename F>
	using Conditional_t=typename detail::Conditional_<Cond>::template type<T, F>;

	namespace detail {
		using int_=std::intmax_t;
	}

	using int_=detail::int_;
	
	////////////////////////
	///Type modifications///
	////////////////////////
	//remove cv specifiers:
	namespace detail {
		template<typename T>
		struct Remove_const_ {
			using type=T;
		};

		template<typename T>
		struct Remove_const_ <T const> {
			using type=T;
		};

		template<typename T>
		struct Remove_volatile_ {
			using type=T;
		};

		template<typename T>
		struct Remove_volatile_<T volatile> {
			using type=T;
		};
	}
	
	template<typename T>
	using Remove_const=detail::Remove_const_<T>;

	template<typename T>
	using Remove_volatile =detail::Remove_volatile_<T>;

	template<typename T>
	using Remove_cv=Remove_volatile<typename Remove_const<T>::type>;

	//add cv-specifiers
	namespace detail {
		template<typename T>
		struct Add_const_ {
			using type=T const;
		};

		template<>
		struct Add_const_<void> {
			using type=void;
		};

		template<typename T>
		struct Add_volatile_ {
			using type=T volatile;
		};

		template<>
		struct Add_volatile_<void> {
			using type=void;
		};

		template<typename T>
		struct Add_cv_ {
			using type=T const volatile;
		};

		template<>
		struct Add_cv_ <void>{
			using type=void;
		};
	}

	template<typename T>
	using Add_const=detail::Add_const_<T>;

	template<typename T>
	using Add_volatile=detail::Add_volatile_<T>;

	template<typename T>
	using Add_cv=detail::Add_cv_<T>;

	template<typename T>
	using Add_const_t=typename detail::Add_const_<T>::type;

	template<typename T>
	using Add_volatile_t=typename detail::Add_volatile_<T>::type;

	template<typename T>
	using Add_cv_t=typename detail::Add_cv_<T>::type;

	//pointers:
	namespace detail {

		template<typename T>
		struct Remove_pointer_ {
			using type=T;
		};

		template<typename T>
		struct Remove_pointer_<T*> {
			using type=T;
		};

		template<typename T>
		struct Remove_pointer_<T* const> {
			using type=T;
		};

		template<typename T>
		struct Remove_pointer_<T* volatile> {
			using type=T;
		};

		template<typename T>
		struct Remove_pointer_<T* const volatile> {
			using type=T;
		};
	}

	template<typename T>
	using Remove_pointer=detail::Remove_pointer_<T>;
	
	template<typename T>
	using Remove_pointer_t=typename detail::Remove_pointer_<T>::type;

	namespace detail {
		template<typename T>
		struct Remove_reference_ {
			using type=T;
		};

		template<typename T>
		struct Remove_reference_<T&> {
			using type=T;
		};

		template<typename T>
		struct Remove_reference_<T&&> {
			using type=T;
		};
	}

	template<typename T>
	using Remove_reference=detail::Remove_reference_<T>;

	//need to provide SFINAE context here
	//because a reference or pointer to a qualified function type member is forbidden,
	//only allow the pointer-to-member
	namespace detail {
		template<typename T,typename =Void_t<>>
		struct Add_pointer_ {
			using type=T;
		};
		
		template<typename T>
		struct Add_pointer_ <T,Void_t<typename Remove_reference<T>::type*>>{
			using type=typename Remove_reference<T>::type*;
		};
	}

	template<typename T>
	using Add_pointer=detail::Add_pointer_<T>;

	template<typename T>
	using Add_pointer_t=typename detail::Add_pointer_<T>::type;

	namespace detail {
		//Based on the saying in Add_pointer,
		//also provide SFINAE context here.
		//Additionally,reference-to-void is forbidden,
		//but SFINAE also include that situation

		//type_identity just fit to the standard writing
		//provide type member:type
		template<typename T>
		struct type_identity {
			using type=T;
		};

		//expression SFINAE
		template<typename T>
		auto try_add_lvalue_reference(void*)
			->type_identity<T&>;

		template<typename T>
		auto try_add_lvalue_reference(...)
			->type_identity<T>;
	
		template<typename T>
		struct Add_lvalue_reference_ :decltype(try_add_lvalue_reference<T>(nullptr)){};

		template<typename T>
		auto try_add_rvalue_reference(void*)
			->type_identity<T&&>;

		template<typename T>
		auto try_add_rvalue_reference(...)
			->type_identity<T>;

		template<typename T>
		struct Add_rvalue_reference_ :decltype(try_add_rvalue_reference<T>(nullptr)){};
	}

	template<typename T>
	using Add_lvalue_reference=detail::Add_lvalue_reference_<T>;

	template<typename T>
	using Add_rvalue_reference=detail::Add_rvalue_reference_<T>;


	template<typename T>
	using Remove_reference_t=typename detail::Remove_reference_<T>::type;

	template<typename T>
	using Remove_const_t=typename detail::Remove_const_<T>::type;

	template<typename T>
	using Remove_volatile_t=typename detail::Remove_volatile_<T>::type;

	template<typename T>
	using Remove_cv_t=Remove_const_t<Remove_volatile_t<T>>;

	template<typename T>
	using Add_lvalue_reference_t=typename detail::Add_lvalue_reference_<T>::type;

	template<typename T>
	using Add_rvalue_reference_t=typename detail::Add_rvalue_reference_<T>::type;

	//dummy object
	template<typename T>
	Add_rvalue_reference_t<T> declval()noexcept;



	//////////////////////////
	///Operation on traits////
	//////////////////////////
	namespace detail {
		template<typename ...>
		struct _or_ {};

		template<>
		struct _or_<> :_false_type {};

		template<typename T>
		struct _or_ <T> :T {};

		template<typename T1, typename T2>
		struct _or_<T1, T2> :Conditional_t<T1::value, T1, T2> {};

		template<typename T1, typename T2, typename T3, typename...Tn>
		struct _or_<T1, T2, T3, Tn...> :Conditional_t<T1::value, T1, _or_<T2, T3, Tn...>> {};

		template<typename...>
		struct _and_ {};

		template<>
		struct _and_<> :_true_type {};

		template<typename T>
		struct _and_<T> :T {};

		template<typename T1, typename T2>
		struct _and_<T1, T2> :Conditional_t<T1::value, T2, T1> {};

		template<typename T1, typename T2, typename T3, typename ...Tn>
		struct _and_<T1, T2, T3, Tn...> :Conditional_t<T1::value, _and_<T2, T3, Tn...>, T1> {};

		template<typename T>
		struct _not_ :Bool_constant<!bool(T::value)> {};
	}
	//variadic logical AND metafunction
	template<typename...Tn>
	struct Conjunction :detail::_and_<Tn...> {};

	//variadic logical OR metafunction
	template<typename...Tn>
	struct Disjunction :detail::_or_<Tn...> {};

	//logical NOT metafunction
	template<typename T>
	struct Negation :detail::_not_<T> {};

	//////////////////////////////////////
	///Check the Primary Type Category////
	//////////////////////////////////////
	
	//check if a type is void
	namespace detail {
		template<typename T>
		struct Is_void_
			:_false_type {};

		template<>
		struct Is_void_<void>
			:_true_type {};

	}

	template<typename T>
	using Is_void=typename detail::Is_void_<Remove_cv_t<T>>::type;


//the helper of Is_integral
#define DEFINE_INTEGRAL(INT_TYPE)								\
	template<>														\
	struct Is_integral_<INT_TYPE>									\
	:_true_type {													\
	};																

	//check if a type is an integral type
	namespace detail {
		template<typename T>
		struct Is_integral_ :_false_type {};

		DEFINE_INTEGRAL(bool)
		DEFINE_INTEGRAL(char)
		DEFINE_INTEGRAL(unsigned char)
		DEFINE_INTEGRAL(signed char)
		DEFINE_INTEGRAL(wchar_t)
		DEFINE_INTEGRAL(char16_t)
		DEFINE_INTEGRAL(char32_t)

		DEFINE_INTEGRAL(signed short)
		DEFINE_INTEGRAL(unsigned short)
		DEFINE_INTEGRAL(signed int)
		DEFINE_INTEGRAL(unsigned int)
		DEFINE_INTEGRAL(signed long)
		DEFINE_INTEGRAL(unsigned long)
		DEFINE_INTEGRAL(signed long long)
		DEFINE_INTEGRAL(unsigned long long)
	}

	template<typename T>
	using Is_integral=typename detail::Is_integral_<Remove_cv_t<T>>::type;

//the helper of Is_floating_point
#define DEFINE_FLOATING_POINT(TYPE)									\
	template<>														\
	struct Is_floating_point_<TYPE> :_true_type{};						

	//check if a type is a floating point type
	namespace detail {
		template<typename T>
		struct Is_floating_point_ :_false_type {};

		DEFINE_FLOATING_POINT(double)
		DEFINE_FLOATING_POINT(double long)
		DEFINE_FLOATING_POINT(float)
	}

	template<typename T>
	using Is_floating_point=typename detail::Is_floating_point_<Remove_cv_t<T>>::type;

	
	//check if a type is an array type
	namespace detail {
		template<typename T>
		struct Is_array_ :_false_type {};

		template<typename T>
		struct Is_array_<T[]> :_true_type {};

		template<typename T, int_ sz>
		struct Is_array_<T[sz]> :_true_type {};
	}

	template<typename T>
	using Is_array=typename detail::Is_array_<Remove_cv_t<T>>::type;

	//check if a type is a pointer type
	namespace detail {
		template<typename T>
		struct Is_pointer_ :_false_type {
		};

		template<typename T>
		struct Is_pointer_ <T*> :_true_type {
		};

	}

	template<typename T>
	using Is_pointer=typename detail::Is_pointer_<Remove_cv_t<T>>::type;


	//check if a type is a std::nullptr_t
	namespace detail {
		template<typename T>
		struct Is_null_pointer_ :_false_type {
		};

		template<>
		struct Is_null_pointer_<std::nullptr_t> :_true_type {};
	}

	template<typename T>
	using Is_null_pointer=typename detail::Is_null_pointer_<Remove_cv_t<T>>::type;


	//check if a type is a pointer-to-member object type
	namespace detail {
		template<typename T>
		struct Is_member_object_pointer_ :_false_type {
		};

		template<typename C, typename T>
		struct Is_member_object_pointer_<T C::*> :_true_type {
			using Member=T;
			using Class=C;
		};
	}

	template<typename T>
	using Is_member_object_pointer=typename detail::Is_member_object_pointer_<Remove_cv_t<T>>::type;

	//used to obtain the member object type and its class to which belongs  
	template<typename T,typename =Is_member_object_pointer<T>>
	struct member_object_pointer_traits {
		using Member=typename detail::Is_member_object_pointer_<T>::Member;
		using Class =typename detail::Is_member_object_pointer_<T>::Class;
	};

	template<typename T>
	struct member_object_pointer_traits <T,_false_type>{
	};


	//check if a type is a pointer-to-member function type
	namespace detail {

		template<typename T>
		struct Is_function_;

		template<typename T>
		struct Is_member_function_pointer_ :_false_type {};

		template<typename F, typename C>
		struct Is_member_function_pointer_<F C::*> : Is_function_<F> {
			using Member=F;
			using Class=C;
		};
	}

	template<typename T>
	using Is_member_function_pointer=typename detail::Is_member_function_pointer_<Remove_cv_t<T>>::type;

	//used to obtain the member function type,Return type,Class type and a variable implies 
	//the member function if has c-style variadic arguments
	template<typename T, bool=Is_member_function_pointer<T>::value>
	struct member_function_pointer_traits {
		using Member=typename detail::Is_member_function_pointer_<T>::Member;
		using Return=typename detail::Is_member_function_pointer_<T>::Return;
		using Class =typename detail::Is_member_function_pointer_<T>::Class;
		static constexpr bool variadic=detail::Is_member_function_pointer_<T>::variadic;
	};

	template<typename T>
	struct member_function_pointer_traits<T, false> {
	};

	
	namespace detail {
		//check if a type is a lvalue reference type
		template<typename T>
		struct Is_lvalue_reference_ :_false_type {};

		template<typename T>
		struct Is_lvalue_reference_<T&> :_true_type {};

		//check if a type is a rvalue reference type
		template<typename T>
		struct Is_rvalue_reference_ :_false_type {};

		template<typename T>
		struct Is_rvalue_reference_<T&&> :_true_type {};
	}

	//don't provide the Remove_cv_t on T,because reference only has low-level const
	template<typename T>
	using Is_lvalue_reference=typename detail::Is_lvalue_reference_<T>::type;

	template<typename T>
	using Is_rvalue_reference=typename detail::Is_rvalue_reference_<T>::type;

	//SFINAE out through Void_t
	namespace detail {
		//check a type is a class type
		template<typename T, typename=Void_t<>>
		struct Is_class_ :_false_type {};

		template<typename T>
		struct Is_class_<T, Void_t<int T::*>>
			:_true_type {};
	}

	//In fact,no need to provide Remove_cv_t here
	//becase the type just a made-up type
	template<typename T>
	using Is_class=typename detail::Is_class_<T>::type;

#if __cplusplus >= 201703L
//the helper of Is_function
#define DEFINE_IS_FUNCTION(TYPE)									\
	template<typename R, typename ...Paras>								\
	struct Is_function_<R(Paras...) TYPE> :_true_type {					\
		using Return=R;													\
		using Parameter=mpl::Typelist<Paras...>;						\
		static constexpr bool variadic=false;							\
	};

//variadic helper of Is_function
#define DEFINE_IS_FUNCTION_VARG(TYPE)								\
	template<typename R, typename ...Paras>								\
	struct Is_function_<R(Paras..., ...) TYPE> :_true_type {			\
		using Return=R;													\
		using Parameter=mpl::Typelist<Paras...>;						\
		static constexpr bool variadic=true;							\
	};
#endif

	namespace detail {
		//check if a type is a function type
		template<typename T>
		struct Is_function_ :_false_type {};

		//total 48 partial specializations
		
		//regular
		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...)> :_true_type {
            using Return=R;
            using Parameter=mpl::Typelist<Paras...>;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...)> :_true_type {
			using Return=R;
            using Parameter=mpl::Typelist<Paras...>;
			static constexpr bool variadic=true;
		};

#if __cplusplus >= 201703L	
		//cv
		DEFINE_IS_FUNCTION(const)
		DEFINE_IS_FUNCTION(volatile)
		DEFINE_IS_FUNCTION(const volatile)

		//&/&&
		DEFINE_IS_FUNCTION(&)
		DEFINE_IS_FUNCTION(const&)
		DEFINE_IS_FUNCTION(volatile&)
		DEFINE_IS_FUNCTION(const volatile&)
		DEFINE_IS_FUNCTION(&&)
		DEFINE_IS_FUNCTION(const&&)
		DEFINE_IS_FUNCTION(volatile&&)
		DEFINE_IS_FUNCTION(const volatile&&)

		//noexcept
		DEFINE_IS_FUNCTION(noexcept)
		DEFINE_IS_FUNCTION(const noexcept)
		DEFINE_IS_FUNCTION(volatile noexcept)
		DEFINE_IS_FUNCTION(const volatile noexcept)
		DEFINE_IS_FUNCTION(&noexcept)
		DEFINE_IS_FUNCTION(const& noexcept)
		DEFINE_IS_FUNCTION(volatile& noexcept)
		DEFINE_IS_FUNCTION(const volatile& noexcept)
		DEFINE_IS_FUNCTION(&& noexcept)
		DEFINE_IS_FUNCTION(const&& noexcept)
		DEFINE_IS_FUNCTION(volatile&& noexcept)
		DEFINE_IS_FUNCTION(const volatile&& noexcept)

		//variadic
		DEFINE_IS_FUNCTION_VARG(volatile)
		DEFINE_IS_FUNCTION_VARG(const volatile)
		DEFINE_IS_FUNCTION_VARG(&)
		DEFINE_IS_FUNCTION_VARG(const&)
		DEFINE_IS_FUNCTION_VARG(volatile&)
		DEFINE_IS_FUNCTION_VARG(const volatile&)
		DEFINE_IS_FUNCTION_VARG(&&)
		DEFINE_IS_FUNCTION_VARG(const&&)
		DEFINE_IS_FUNCTION_VARG(volatile&&)
		DEFINE_IS_FUNCTION_VARG(const volatile&&)
		DEFINE_IS_FUNCTION_VARG(noexcept)
		DEFINE_IS_FUNCTION_VARG(const noexcept)
		DEFINE_IS_FUNCTION_VARG(volatile noexcept)
        DEFINE_IS_FUNCTION_VARG(const volatile noexcept)
		DEFINE_IS_FUNCTION_VARG(&noexcept)
		DEFINE_IS_FUNCTION_VARG(const& noexcept)
		DEFINE_IS_FUNCTION_VARG(volatile& noexcept)
		DEFINE_IS_FUNCTION_VARG(const volatile& noexcept)
		DEFINE_IS_FUNCTION_VARG(&& noexcept)
		DEFINE_IS_FUNCTION_VARG(const&& noexcept)
		DEFINE_IS_FUNCTION_VARG(volatile&& noexcept)
		DEFINE_IS_FUNCTION_VARG(const volatile&& noexcept)
		DEFINE_IS_FUNCTION_VARG(const)
#endif
	}

	//no need to provide Remove_cv_t,because the cv specifiers have different means on function
	//moreover,function type can't be used to declare an object
	template<typename T>
	using Is_function=typename detail::Is_function_<T>::type;

	template<typename T, bool=Is_function<T>::value>
	struct function_traits {
		using Return=typename detail::Is_function_<T>::Return;
        using Parameter=typename detail::Is_function_<T>::Parameter;
		static constexpr bool variadic=detail::Is_function_<T>::variadic;
	};

	template<typename T>
	struct function_traits<T, false> {
	};

	//check if a type an enumeration type
	namespace detail {
		template<typename T>
		using Is_enum_=Disjunction<Is_void<T>, Is_null_pointer<T>, Is_integral<T>, Is_floating_point<T>,
								Is_array<T>, Is_pointer<T>, Is_class<T>,
								Is_lvalue_reference<T>, Is_rvalue_reference<T>,
								Is_member_object_pointer<T>, Is_member_function_pointer<T>,
								Is_function<T>>;
	}

	template<typename T>
	using Is_enum=typename Negation<detail::Is_enum_<T>>::type;

	/////////////////////////////////////
	///Composite type categories/////////
	/////////////////////////////////////
	//arithmetic type include the integral type and the floating point type
	template<typename T>
	using Is_arithmetic=typename Disjunction<Is_integral<T>, Is_floating_point<T>>::type;

	//fundamental type include the arithmetic type,the void type and the std::nullptr_t
	template<typename T>
	using Is_fundamental=typename Disjunction<Is_arithmetic<T>, Is_void<T>, Is_null_pointer<T>>::type;

	//member pointer type include the pointer-to-member object and the pointer-to-member function
	template<typename T>
	using Is_member_pointer=typename Disjunction<Is_member_object_pointer<T>, Is_member_function_pointer<T>>::type;

	//scalar type
	template<typename T>
	using Is_scalar=typename Disjunction<Is_arithmetic<T>, Is_pointer<T>, Is_member_pointer<T>,
								Is_enum<T>, Is_null_pointer<T>>::type;

	//reference type include the lvalue reference and rvalue reference
	template<typename T>
	using Is_reference=typename Disjunction<Is_lvalue_reference<T>, Is_rvalue_reference<T>>::type;


	//template<typename T>
	//using Is_object=typename Disjunction<Is_scalar<T>, Is_array<T>, Is_class<T>>::type;

	//why I use this version as following?
	//just because I can't write the "Is_union" though there are instantiations of Is_function...
	namespace detail {
		template<typename T>
		using Is_object_=typename Conjunction<Is_void<T>, Is_reference<T>, Is_function<T>>::type;
	}

	template<typename T>
	using Is_object=typename Negation<detail::Is_object_<T>>::type;

	//fundamemtal include arithmetic,null_pointer,void
	//others are compound types
	//this is,array,function,union,reference,pointer,pointer to member,class,union,enumeration
	template<typename T>
	using Is_compound=typename Negation<Is_fundamental<T>>::type;

	////////////////////////
	///Type properties//////
	////////////////////////
	namespace detail {
		template<typename T>
		struct Is_const_ :_false_type {
		};

		template<typename T>
		struct Is_const_ <T const> :_true_type {
		};

		template<typename T>
		struct Is_const_<T const*> :_true_type {
		};
	}

	template<typename T>
	using Is_const=typename detail::Is_const_<T>::type;

	namespace detail {
		template<typename T>
		struct Is_volatile_ :_false_type {};

		template<typename T>
		struct Is_volatile_<T volatile> :_true_type {};

		template<typename T>
		struct Is_volatile_<T volatile*> :_true_type {};
	}

	template<typename T>
	using Is_volatile=typename detail::Is_volatile_<T>::type;

	//Is_trivial
	//Is_trivially_copyable
	//Is_standard_layout
	//Is_literal_type

	template<typename T>
	using Is_pod=typename _type_traits<T>::is_POD_type;

	template<typename T>
	using Is_trivially_default_constructible=typename _type_traits<T>::has_trivially_default_constructor;

	template<typename T>
	using Is_trivially_copy_constructible=typename _type_traits<T>::has_trivially_copy_constructble;

	template<typename T>
	using Is_trivially_assigment=typename _type_traits<T>::has_trivially_assigment_operator;

	template<typename T>
	using Is_trivially_destructible=typename _type_traits<T>::has_trivially_destructor;

	namespace detail {
		template<typename T, typename=Void_t<>>
		struct Is_empty_ :_false_type {};

		template<typename T>
		struct Is_empty_<T, Void_t<decltype(sizeof(T)==1)>> :_true_type {};
	}

	template<typename T>
	using Is_empty=typename detail::Is_empty_<T>::type;

	namespace detail {
		//use the property of dynamic_cast
		template<typename T>
		_true_type Is_polymorphic_(
			decltype(dynamic_cast<const volatile void*>(static_cast<T*>(nullptr)))
		);

		template<typename T>
		_false_type Is_polymorphic_(...);
	}

	template<typename T>
	using Is_polymorphic=decltype(detail::Is_polymorphic_<T>(nullptr));

	namespace detail {
		template<typename T, typename=Conjunction<Is_class<T>, Is_polymorphic<T>>>
		struct Is_abstract_ :_true_type {};

		template<typename T>
		struct Is_abstract_<T, _false_type> :_false_type {};
	}

	template<typename T>
	using Is_abstract=typename detail::Is_abstract_<T>::type;

	//Is_aggregate
	//Is_fianl

	namespace detail {
		template<typename T, bool=Is_arithmetic<T>::value>
		struct Is_signed_ :Bool_constant<(bool)(T(-1)<T(0))>{};

		template<typename T>
		struct Is_signed_<T, false> :_false_type {};
	}

	template<typename T>
	using Is_signed=typename detail::Is_signed_<T>::type;

	namespace detail {
		template<typename T, bool=Is_arithmetic<T>::value>
		struct Is_unsigned_ :Bool_constant<(bool)(T(0)<T(-1))>{};

		template<typename T>
		struct Is_unsigned_<T, false> :_false_type {};
	}

	template<typename T>
	using Is_unsigned=typename detail::Is_unsigned_<T>::type;

	namespace detail {
		template<typename T>
		struct Is_bounded_array_ :_false_type {};

		template<typename T, int_ sz>
		struct Is_bounded_array_<T[sz]> :_true_type {
		};

		template<typename T>
		struct Is_unbounded_array_ :_false_type {};

		template<typename T>
		struct Is_unbounded_array_<T[]> :_true_type {};
	}

	template<typename T>
	using Is_bounded_array=typename detail::Is_bounded_array_<T>::type;

	template<typename T>
	using Is_unbounded_array=typename detail::Is_unbounded_array_<T>::type;

	/*namespace detail {
		template<typename T,bool =Is_enum<T>::value>
		struct Is_scoped_enum:Bool_constant<Is_convertible<T,std::underling_type_t<T>>::value>{};

		template<typename T>
		struct Is_scoped_enum<T, false> :_false_type {};
	}*/

	////////////////////////
	///Property quries//////
	////////////////////////
	namespace detail {
		template<typename T>
		struct Rank_ {
			static constexpr int_ value=0;
		};

		template<typename T>
		struct Rank_<T[]> {
			static constexpr int_ value=Rank_<T>::value+1;
		};

		template<typename T, int_ sz>
		struct Rank_<T[sz]> {
			static constexpr int_ value=Rank_<T>::value+1;
		};
	}

	template<typename T>
	using Rank=detail::Rank_<T>;

	namespace detail {
		template<typename T, int_ N>
		struct Extent_ {
			static constexpr int_ value=0;
		};

		template<typename T>
		struct Extent_<T[], 0> {
			static constexpr int_ value=0;
		};

		template<typename T, int_ N>
		struct Extent_<T[], N> {
			static constexpr int_ value=Extent_<T, N-1>::value;
		};
		template<typename T, int_ sz>
		struct Extent_<T[sz], 0> {
			static constexpr int_ value=sz;
		};

		template<typename T, int_ N, int_ sz>
		struct Extent_<T[sz], N> {
			static constexpr int_ value=Extent_<T, N-1>::value;
		};
	}

	template<typename T, int_ N>
	using Extent=detail::Extent_<T, N>;

	///////////////////////////
	///Supported operations////
	///////////////////////////

#if __cplusplus >= 201703L	
	namespace detail {
		//generic lambda version
		//helper:check validity of f(args...) for F f and Args...args
		template<typename F, typename...Args, typename=
			decltype(declval<F>()(declval<Args&&>()...))>
			_true_type Is_valid_impl(void*);

		template<typename F, typename...Args>
		//template<typename ...args>
		_false_type Is_valid_impl(...);

		//traits factory
		inline constexpr
			auto Is_valid=[](auto f) {
			return [](auto&&... Args) {
				return decltype(Is_valid_impl<decltype(f),
					decltype(Args)&& ...
				>(nullptr)){};
			};
		};

		//helper template to present a type as a value
		template<typename T>
		struct typeT {
			using type=T;
		};

		//helper to wrap a type as a value
		template<typename T>
		constexpr auto type=typeT<T>{};

		//helper to unwrap a wrapped type in unevaluated context
		template<typename T>
		T valueT(typeT<T>);  //no definition needed

		//typeT--->[valueT]--->T

		
		constexpr auto Is_default_constructible_
			=Is_valid([](auto x)->decltype((void)decltype(
				valueT(x))()) {
				});

		/*constexpr auto Is_destructible_
			=Is_valid([](auto x)->decltype((void)(declval<decltype(
				valueT(x))>().~decltype(valueT(x))())) {
				});*/

		constexpr auto Is_constructible_
			=Is_valid([](auto x,auto ...args)->decltype((void)(decltype(
				valueT(x))(declval<decltype(valueT(args))>()...))) {
				});

		constexpr auto Is_assignable_
			=Is_valid([](auto x, auto y)->decltype((void)(
				declval<decltype(valueT(x))>() = declval<decltype(valueT(y))>())) {
				});

		constexpr auto Is_swappable_with_
			=Is_valid([](auto x, auto y)->decltype((void)(
				swap(declval<decltype(valueT(x))>(), declval<decltype(valueT(y))>()),
				swap(declval<decltype(valueT(y))>(), declval<decltype(valueT(x))>()))) {
				});


		template<typename T>
		struct Is_destructible_Helper {
		private:
			//declval<U&>().~U()
			//express p->~U(),i.e. (*p).~U() requiring p is a lvalue
			template<typename U, typename=decltype(declval<U&>().~U())>
			static _true_type test(void*);
			template<typename>
			static _false_type test(...);
		public:
			using type=decltype(test<T>(nullptr));
		};

	} //namespace detail

	//type
	template<typename T>
	using Is_default_constructible
		=decltype(detail::Is_default_constructible_(detail::type<T>));

	template<typename T,typename ...Types>
	using Is_constructible
		=decltype(detail::Is_constructible_(detail::type<T>,detail::type<Types>...));

	template<typename T>
	using Is_copy_constructible
		=Is_constructible<T, T const&>;

	template<typename T>
	using Is_move_constructible
		=Is_constructible<T, T&&>;


	template<typename T,typename U>
	using Is_assignable
		=decltype(detail::Is_assignable_(detail::type<T>, detail::type<U>));

	template<typename T>
	using Is_copy_assignable
		=Is_assignable<T,const T&>;

	template<typename T>
	using Is_move_assignable
		=Is_assignable<T,T&&>;

	template<typename T>
	using Is_destructible
		=typename detail::Is_destructible_Helper<T>::type;

	template<typename T,typename U>
	using Is_swappable_with
		=decltype(detail::Is_swappable_with_(detail::type<T>, detail::type<U>));

	template<typename T,typename =Void_t<>>
	struct Is_swappable
		:_false_type {};

	template<typename T>
	struct Is_swappable<T, Void_t<T&>>
		:Is_swappable_with<T&, T&> {};

	//type-nothrow
	template<typename T,typename =Is_default_constructible<T>>
	struct Is_nothrow_default_constructible
		:Bool_constant<noexcept(T{})>{};

	template<typename T>
	struct Is_nothrow_default_constructible <T, _false_type>
		:_false_type {};

	template<typename T,typename =Is_destructible<T>>
	struct Is_nothrow_destructible
		:Bool_constant<noexcept(declval<T>().~T())> {};

	template<typename T>
	struct Is_nothrow_destructible<T,_false_type>
		:_false_type {};


    namespace detail{
        template<bool ,typename T,typename ...Types>
        struct Is_nothrow_constructible_:_false_type{
        };

        template<typename T,typename ...Types>
        struct Is_nothrow_constructible_<true,T,Types...>
            :Bool_constant<noexcept(T(declval<Types>()...))>{};

        template<typename T,typename ...Types>
        struct Is_nothrow_constructible_helper
            :Is_nothrow_constructible_<Is_constructible<T,Types...>::value,T,Types...>{
        };
    }

	template<typename T,typename ...Types>
	using Is_nothrow_constructible
        =typename detail::Is_nothrow_constructible_helper<T, Types...>::type;

	template<typename T,typename =Is_copy_constructible<T>>
	struct Is_nothrow_copy_constructible
		: Bool_constant<noexcept(T(declval<T const&>()))>{};

	template<typename T>
	struct Is_nothrow_copy_constructible<T, _false_type> 
		:_false_type{};

	template<typename T,typename =Is_move_constructible<T>>
	struct Is_nothrow_move_constructible
		:Bool_constant<noexcept(T(declval<T&&>()))>{};

	template<typename T>
	struct Is_nothrow_move_constructible<T, _false_type> 
		:_false_type{};

	
	template<typename T, typename U,typename =Is_assignable<T, U>>
	struct Is_nothrow_assignable
		:Bool_constant<noexcept(declval<T>()=declval<U>())> {};

	template<typename T,typename U>
	struct Is_nothrow_assignable<T, U,_false_type>
		:_false_type {};

	template<typename T,typename =Is_copy_assignable<T>>
	struct Is_nothrow_copy_assignable
		:Bool_constant<noexcept(declval<T>()=declval<T const&>())> {};

	template<typename T>
	struct Is_nothrow_copy_assignable<T, _false_type>
		:_false_type {};

	template<typename T,typename =Is_move_assignable<T>>
	struct Is_nothrow_move_assignable
		:Bool_constant<noexcept(declval<T>()=declval<T&&>())> {};

	template<typename T>
	struct Is_nothrow_move_assignable<T, _false_type>
		:_false_type {};

	template<typename T,typename U,typename =Is_swappable_with<T,U>>
	struct Is_nothrow_swappable_with
		: Bool_constant<noexcept(swap(declval<T>(),declval<U>()))
						&&noexcept(swap(declval<U>(),declval<T>()))>{};

	template<typename T>
	struct Is_nothrow_swappable_with<T,_false_type>
		:_false_type {};

	template<typename T, typename =typename Is_swappable<T>::type>
	struct Is_nothrow_swappable :
		Bool_constant<noexcept(swap(declval<T&>(), declval<T&>()))> {};

	template<typename T>
	struct Is_nothrow_swappable<T,_false_type>
		:_false_type{};

#elif __cplusplus >= 201103L
	namespace detail{
#define Object_SUPPORTED_OPERATION_SFINAE(name, condition) \
		template<typename T> \
		struct name##_ { \
		private: \
			template<typename U, typename = decltype(condition)> \
			static _true_type Apply(void*); \
 \
			template<typename > \
			static _false_type Apply(...); \
		public: \
			using type = decltype(Apply<T>(nullptr)); \
		}; \
\
		template<typename T> \
		struct name##T : name##_<T>::type \
		{ };

		Object_SUPPORTED_OPERATION_SFINAE(Is_default_constructible, U{})
		Object_SUPPORTED_OPERATION_SFINAE(Is_destructible, declval<U>().~U())
		
		// Is_contructible better use SFINAE out function
		// because variadic template parameters only allowed in the tain of template parameters list
		template<typename T, typename ...Args>
		struct Is_constructible_ {
			template<typename U,  typename = decltype(U{ declval<Args>()... })>
			static _true_type Apply(void*);

			template<typename>
			static _false_type Apply(...);

		public:
			using type = decltype(Apply<T>(nullptr));
		};

		template<typename T, typename ...Args>
		struct Is_constructibleT : Is_constructible_<T, Args...>::type
		{ };
		
		template<typename T>
		struct Is_copy_constructibleT : Is_constructibleT<T, T const&>::type
		{ };

		template<typename T>
		struct Is_move_constructibleT : Is_constructibleT<T, T&&>::type
		{ };
		
		template<typename T, typename U, typename = void>
		struct Is_assignableT : _false_type
		{ };
		
		template<typename T, typename U>
		struct Is_assignableT<T, U, Void_t<decltype(declval<T>().operator=(declval<U>()))>>
			: _true_type
		{ };
		
		template<typename T>
		struct Is_copy_assignableT : Is_assignableT<T, T const&>
		{ };

		template<typename T>
		struct Is_move_assignableT : Is_assignableT<T, T&&>
		{ };
		
		template<typename T, typename U, typename = void>
		struct Is_swappable_withT : _false_type
		{ };
	
		template<typename T, typename U>
		struct Is_swappable_withT<T, U, Void_t<
			decltype(swap(declval<T>(), declval<U>())),
			decltype(swap(declval<U>(), declval<T>()))
			>>
			: _true_type
		{ };
		
		template<typename T>
		struct Is_swappableT : Is_swappable_withT<T&, T&>
		{ };

		// Trick: set default typename argument to _true_type
		// then if the same typename argument in partial specialization also get _true_type, 
		// because specialization is more "specialized" than primary template
		// so select it
		// (but, if two or more partial specializations get same result
		// maybe trigger fatal error...)
#define NOTHROW_PARTIAL_SPECIALIZATION(name, condition) \
		template<typename T, typename = _true_type> \
		struct Is_nothrow_##name : _false_type {}; \
\
		template<typename T> \
		struct Is_nothrow_##name<T, typename Is_##name<T>::type> \
			: Bool_constant<noexcept(condition)> {};

		NOTHROW_PARTIAL_SPECIALIZATION(default_constructibleT, T{})	
		NOTHROW_PARTIAL_SPECIALIZATION(destructibleT, declval<T>().~T())

		template<typename T, typename ...Args>
		struct Is_nothrow_constructible_ {
		private:
			template<typename U, typename ...Args2>
			static Bool_constant<noexcept(U{ declval<Args2>()... })> Apply(_true_type);

			template<typename , typename ...>
			static _false_type Apply(_false_type);

		public:
			using type = decltype(Apply<T, Args...>(typename Is_constructibleT<T, Args...>::type{}));
		};
	
		template<typename T, typename ...Args>
		struct Is_nothrow_constructibleT : Is_nothrow_constructible_<T, Args...>::type {};

		template<typename T>
		struct Is_nothrow_copy_constructibleT : Is_nothrow_constructibleT<T, T const&> {};

		template<typename T>
		struct Is_nothrow_move_constructibleT : Is_nothrow_constructibleT<T, T&&> {};
	
		template<typename T, typename U, typename>
		struct Is_nothrow_assignable_ : _false_type {};

		template<typename T, typename U>
		struct Is_nothrow_assignable_<T, U, _true_type>
			: Bool_constant<noexcept(declval<T>().operator=(declval<U>()))> {};

		template<typename T, typename U>
		struct Is_nothrow_assignableT : Is_nothrow_assignable_<T, U, typename Is_assignableT<T, U>::type> {};

		template<typename T>
		struct Is_nothrow_copy_assignableT : Is_nothrow_assignableT<T, T const&> {};

		template<typename T>
		struct Is_nothrow_move_assignableT : Is_nothrow_assignableT<T, T&&> {};

		template<typename T, typename U, typename = _true_type>
		struct Is_nothrow_swappable_withT : _false_type {};

		template<typename T, typename U>
		struct Is_nothrow_swappable_withT<T, U, typename Is_swappable_withT<T, U>::type>
			: Bool_constant<
			noexcept(swap(declval<T>(), declval<U>())) &&
			noexcept(swap(declval<U>(), declval<T>()))
			> {};

		template<typename T>
		struct Is_nothrow_swappableT : Is_nothrow_swappable_withT<T&, T&> {};

	} // namespace detail

	// type alias
	template<typename T>
	using Is_default_constructible = detail::Is_default_constructibleT<T>;
	
	template<typename T>
	using Is_destructible  = detail::Is_destructibleT<T>;

	template<typename T, typename ...Args>
	using Is_constructible = detail::Is_constructibleT<T, Args...>;
	
	template<typename T>
	using Is_copy_constructible = detail::Is_copy_constructibleT<T>;

	template<typename T>
	using Is_move_constructible = detail::Is_move_constructibleT<T>;

	template<typename T, typename U>
	using Is_assignable = detail::Is_assignableT<T, U>;

	template<typename T>
	using Is_copy_assignable = detail::Is_copy_assignableT<T>;

	template<typename T>
	using Is_move_assignable = detail::Is_move_assignableT<T>;
	
	template<typename T, typename U>
	using Is_swappable_with = detail::Is_swappable_withT<T, U>;

	template<typename T>
	using Is_swappable = detail::Is_swappableT<T>;

	template<typename T>
	using Is_nothrow_default_constructible = detail::Is_nothrow_default_constructibleT<T>;

	template<typename T>
	using Is_nothrow_destructible = detail::Is_nothrow_destructibleT<T>;

	template<typename T, typename ...Args>
	using Is_nothrow_constructible = detail::Is_nothrow_constructibleT<T, Args...>;

	template<typename T>
	using Is_nothrow_copy_constructible = detail::Is_nothrow_copy_constructibleT<T>;

	template<typename T>
	using Is_nothrow_move_constructible = detail::Is_nothrow_move_constructibleT<T>;
	
	template<typename T, typename U>
	using Is_nothrow_assignable = detail::Is_nothrow_assignableT<T, U>;

	template<typename T>
	using Is_nothrow_copy_assignable = detail::Is_nothrow_copy_assignableT<T>;

	template<typename T>
	using Is_nothrow_move_assignable = detail::Is_nothrow_move_assignableT<T>;

	template<typename T, typename U>
	using Is_nothrow_swappable_with = detail::Is_nothrow_swappable_withT<T, U>;

	template<typename T>
	using Is_nothrow_swappable = detail::Is_nothrow_swappableT<T>;

#endif // __cplusplus > 201703L

	////////////////////////
	///Type relationships///
	////////////////////////
	namespace detail {
		template<typename from, typename to, bool =
			Disjunction<
				Is_void<to>,
				Is_array<to>, 
				Is_function<to>
			>::value>
			struct Is_convertible_ :Bool_constant<Is_void<to>::value||Is_void<from>::value> {
		};

		template<typename from, typename to>
		struct Is_convertible_<from, to, false> {
		private:
			static void aux(to);
			template<typename F,
				typename=decltype(aux(declval<F>()))>
				static _true_type test(void*);
			template<typename>
			static _false_type test(...);
		public:
			using type=decltype(test<from>(nullptr));
		};
	}

	template<typename from, typename to>
	using Is_convertible =typename detail::Is_convertible_<from, to>::type;

	namespace detail {
#if __cplusplus < 201402L
		template<typename T1, typename T2>
		struct Is_sameT :_false_type {
		};

		template<typename T>
		struct Is_sameT<T, T> :_true_type {
		};
		
#else	
		//Based on the rule of chiel
		template<typename T1,typename T2>
		constexpr bool Is_same_=false;

		template<typename T>
		constexpr bool Is_same_<T,T> =true;

		template<typename T, typename U>
		struct Is_sameT : Bool_constant<Is_same_<T, U>> {};
#endif
	}
	
	template<typename T, typename U>
	using Is_same = detail::Is_sameT<T, U>;

	namespace detail {
		template<typename B, typename D, bool =Conjunction<
			Is_class<B>,
			Is_class<D>
			>::value>
		struct Is_base_of_ :Bool_constant<Is_convertible<D*, B*>::value> {};

		template<typename B,typename D>
		struct Is_base_of_<B, D, false> :_false_type {};
	}
	
	template<typename B,typename D>
	using Is_base_of=typename detail::Is_base_of_<B, D>::type;
	
//helper of Make_signed
#define MAKE_SIGNED_HELPER(TYPE)									\
	template<>															\
	struct Make_signed_<unsigned TYPE, false> {							\
		using type=signed TYPE;											\
	};

//helper of Make_unsigned
#define	MAKE_UNSIGNED_HELPER(TYPE)									\
	template<>															\
	struct Make_unsigned_<signed TYPE,false>{							\
		using type=unsigned TYPE;										\
	};

#define MAKE_UNSIGNED_HELPER_(TYPE)									\
	template<>															\
	struct Make_unsigned_<TYPE,false>{									\
		using type=unsigned TYPE;										\
	};


	//sign modifiers:
	namespace detail {
		template<typename T,bool =Is_signed<T>::value>
		struct Make_signed_ {
			using type=T;
		};

		MAKE_SIGNED_HELPER(char)
		MAKE_SIGNED_HELPER(int)
		MAKE_SIGNED_HELPER(short)
		MAKE_SIGNED_HELPER(long)
		MAKE_SIGNED_HELPER(long long)

		template<typename T,bool =Is_unsigned<T>::value>
		struct Make_unsigned_ {
			using type=T;
		};

		MAKE_UNSIGNED_HELPER(int)
		MAKE_UNSIGNED_HELPER(char)
		MAKE_UNSIGNED_HELPER(short)
		MAKE_UNSIGNED_HELPER(long)
		MAKE_UNSIGNED_HELPER(long long)

		MAKE_UNSIGNED_HELPER_(char)
		
	}


	template<typename T>
	using Make_signed=detail::Make_signed_<T>;

	template<typename T>
	using Make_unsigned=detail::Make_unsigned_<T>;

	template<typename T>
	using Make_signed_t=typename detail::Make_signed_<T>::type;

	template<typename T>
	using Make_unsigned_t=typename detail::Make_unsigned_<T>::type;


	//array:
	namespace detail {
		template<typename T>
		struct Remove_extent_ {
			using type=T;
		};

		template<typename T>
		struct Remove_extent_<T[]> {
			using type=T;
		};

		template<typename T, int_ sz>
		struct Remove_extent_<T[sz]> {
			using type=T;
		};


		template<typename T>
		struct Remove_all_extents_ {
			using type=T;
		};

		template<typename T>
		struct Remove_all_extents_<T[]> {
			using type=typename Remove_all_extents_<T>::type;
		};

		template<typename T, int_ sz>
		struct Remove_all_extents_<T[sz]> {
			using type=typename Remove_all_extents_<T>::type;
		};
	}

	template<typename T>
	using Remove_extent=detail::Remove_extent_<T>;

	template<typename T>
	using Remove_all_extents=detail::Remove_all_extents_<T>;

	template<typename T>
	using Remove_extent_t=typename detail::Remove_extent_<T>::type;

	template<typename T>
	using Remove_all_extents_t=typename detail::Remove_all_extents_<T>::type;



	////////////////////////////////////
	///Miscellaneous transformation/////
	////////////////////////////////////

	namespace detail {
		template<bool B, typename T=void>
		struct Enable_if_;

		template<typename T>
		struct Enable_if_ <true, T> {
			using type=T;
		};
	}

	template<bool Cond,typename T=void>
	using Enable_if_t=typename detail::Enable_if_<Cond,T>::type;

	namespace detail {
		//if condition bool value is true,select the second template parameter
		//otherwise,select the third template parameter
		/*template<bool ,typename T,typename F>
		struct Conditional {
			using type=T;
		};

		template<typename T,typename F>
		struct Conditional<false, T, F> {
			using type=F;
		};

		template<bool Cond,typename T,typename F>
		using Conditional_t=typename Conditional<Cond, T, F>::type;*/

		//according to the rule of chiel,use alias template to optimize conditional<>
		template<bool>
		struct Conditional_ {
			template<typename T, typename F>
			using type=T;
		};

		template<>
		struct Conditional_<false> {
			template<typename T, typename F>
			using type=F;
		};
	}
	
	//template<bool B>
	//using Conditional=detail::Conditional_<B>;

	namespace detail {
		template<typename T>
		using R_ref=Remove_reference_t<T>;

		template<typename T, typename U=R_ref<T>>
		struct Decay_{
			using type=Conditional_t<Is_array<U>::value,Remove_extent_t<U>*,
				Conditional_t<Is_function<U>::value, Add_pointer_t<U>,
				Remove_reference_t<U>>>;
		};
	}

	template<typename T>
	using Decay=detail::Decay_<T>;

	template<typename T>
	using Decay_t=typename detail::Decay_<T>::type;

	namespace detail {
		//common_type
		template<typename... >
		struct Common_type_ {};

		template<typename T>
		struct Common_type_<T> :Common_type_<T,T>{
		};

		template<typename T1,typename T2>
		using cont=decltype(true?declval<T1>():declval<T2>());

		template<typename T1, typename T2,typename =Void_t<>>
		struct Common_type_2_impl {
		};

		template<typename T1, typename T2>
		struct Common_type_2_impl<T1, T2, Void_t<cont<T1, T2>>> {
			using type=Decay_t<cont<T1, T2>>;
		};

		template<typename T1,typename T2>
		struct Common_type_<T1,T2>
			:Common_type_2_impl<Decay_t<T1>,Decay_t<T2>>{};

		template<typename AlwasyVoid,typename T1,typename T2,typename ...Rest>
		struct Common_type_multi_impl{};

		template<typename T1,typename T2,typename ...Rest>
		struct Common_type_multi_impl<Void_t<typename Common_type_<T1, T2>::type>, T1, T2, Rest...> 
			:Common_type_<typename Common_type_<T1,T2>::type,Rest...>{};

		template<typename T1,typename T2,typename ...Rest>
		struct Common_type_<T1,T2,Rest...>
			:Common_type_multi_impl<void,T1,T2,Rest...>{};
	}

	template<typename... Ts>
	using Common_type=detail::Common_type_<Ts...>;

	template<typename... Ts>
	using Common_type_t=typename detail::Common_type_<Ts...>::type;

// template variable since c++14
#if __cplusplus >= 201402L
	template<typename...Tn>
	constexpr bool Conjunction_v=Conjunction<Tn...>::value;

	template<typename...Tn>
	constexpr bool Disjunction_v=Disjunction<Tn...>::value;

	template<typename T>
	constexpr bool Negation_v=Negation<T>::value;

	template<typename T>
	constexpr bool Is_void_v=Is_void<T>::value;

	template<typename T>
	constexpr bool Is_integral_v=Is_integral<T>::value;

	template<typename T>
	constexpr bool Is_floating_point_v=Is_floating_point<T>::value;

	template<typename T>
	constexpr bool Is_pointer_v=Is_pointer<T>::value;

	template<typename T>
	constexpr bool Is_array_v=Is_array<T>::value;

	template<typename T>
	constexpr bool Is_null_pointer_v=Is_null_pointer<T>::type;

	template<typename T>
	constexpr bool Is_member_object_pointer_v=Is_member_object_pointer<T>::value;

	template<typename T>
	constexpr bool Is_member_function_pointer_v=Is_member_function_pointer<T>::value;

	template<typename T>
	constexpr bool Is_function_v=Is_function<T>::value;

	template<typename T>
	constexpr bool Is_lvalue_reference_v=Is_lvalue_reference<T>::value;

	template<typename T>
	constexpr bool Is_rvalue_reference_v=Is_rvalue_reference<T>::value;

	template<typename T>
	constexpr bool Is_class_v=Is_class<T>::value;

	template<typename T>
	constexpr bool Is_enum_v=Is_enum<T>::value;
		
	template<typename T>
	constexpr bool Is_arithmetic_v=Is_arithmetic<T>::value;

	template<typename T>
	constexpr bool Is_const_v=Is_const<T>::value;

	template<typename T>
	constexpr bool Is_volatile_v=Is_volatile<T>::value;

	template<typename T>
	constexpr bool Is_pod_v=Is_pod<T>::value;

	template<typename T>
	constexpr bool Is_trivially_default_constructible_v
		=Is_trivially_default_constructible<T>::value;

	template<typename T>
	constexpr bool Is_trivially_copy_constructible_v
		=Is_trivially_copy_constructible<T>::value;

	template<typename T>
	constexpr bool Is_trivially_assignment_v
		=Is_trivially_assigment<T>::value;

	template<typename T>
	constexpr bool Is_trivially_destructible_v
		=Is_trivially_destructible<T>::value;

	template<typename T>
	constexpr bool Is_empty_v=Is_empty<T>::value;

	template<typename T>
	constexpr bool Is_polymorphic_v=Is_polymorphic<T>::value;

	template<typename T>
	constexpr bool Is_abstract_v=Is_abstract<T>::value;

	template<typename T>
	constexpr bool Is_signed_v=Is_signed<T>::value;

	template<typename T>
	constexpr bool Is_unsigned_V=Is_unsigned<T>::value;

	template<typename T>
	constexpr bool Is_unbounded_array_v=Is_unbounded_array<T>::value;

	template<typename T>
	constexpr bool Is_bounded_array_v=Is_bounded_array<T>::value;
	template<typename T>
	constexpr bool Is_fundamental_v=Is_fundamental<T>::value;

	template<typename T>
	constexpr bool Is_member_pointer_v=Is_member_pointer<T>::value;

	template<typename T>
	constexpr bool Is_scalar_v=Is_scalar<T>::value;

	template<typename T>
	constexpr bool Is_reference_v=Is_reference<T>::value;

	template<typename T>
	constexpr bool Is_object_v=Is_object<T>::value;

	template<typename T>
	constexpr bool Is_compound_v=Is_compound<T>::value;

	template<typename T>
	constexpr auto Rank_v=detail::Rank_<T>::value;

	template<typename T, int_ N>
	constexpr auto Extent_v=detail::Extent_<T, N>::value;

	template<typename T>
	constexpr bool Is_default_constructible_v
		=Is_default_constructible<T>::value;


	template<typename T,typename ...Types>
	constexpr bool Is_constructible_v
		=Is_constructible<T,Types...>::value;

	template<typename T>
	constexpr bool Is_copy_constructible_v
		=Is_copy_constructible<T>::value;

	template<typename T>
	constexpr bool Is_move_constructible_v
		=Is_move_constructible<T>::value;

	template<typename T,typename U>
	constexpr bool Is_assignable_v
		=Is_assignable<T,U>::value;

	template<typename T>
	constexpr bool Is_copy_assignable_v
		=Is_copy_assignable<T>::value;

	template<typename T>
	constexpr bool Is_move_assignable_v
		=Is_move_assignable<T>::value;

	template<typename T>
	constexpr bool Is_destructible_v
		=Is_destructible<T>::value;

	template<typename T,typename U>
	constexpr bool Is_swappable_with_v
		=Is_swappable_with<T, U>::value;
	
	template<typename T>
	constexpr bool Is_swappable_v
		=Is_swappable<T>::value;

	//nothrow-variable
	template<typename T>
	constexpr bool Is_nothrow_default_constructible_v
		=Is_nothrow_default_constructible<T>::value;

	template<typename T,typename ...Types>
	constexpr bool Is_nothrow_constructible_v
		=Is_nothrow_constructible<T,Types...>::value;

	template<typename T>
	constexpr bool Is_nothrow_copy_constructible_v
		=Is_nothrow_copy_constructible<T>::value;

	template<typename T>
	constexpr bool Is_nothrow_move_constructible_v
		=Is_nothrow_move_constructible<T>::value;

	template<typename T,typename U>
	constexpr bool Is_nothrow_assignable_v
		=Is_nothrow_assignable<T,U>::value;

	template<typename T>
	constexpr bool Is_nothrow_copy_assignable_v
		=Is_nothrow_copy_assignable<T>::value;

	template<typename T>
	constexpr bool Is_nothrow_move_assignable_v
		=Is_nothrow_move_assignable<T>::value;

	template<typename T>
	constexpr bool Is_nothrow_destructible_v
		=Is_nothrow_destructible<T>::value;

	template<typename T,typename U>
	constexpr bool Is_nothrow_swappable_with_v
		=Is_nothrow_swappable_with<T, U>::value;
	
	template<typename T>
	constexpr bool Is_nothrow_swappable_v
		=Is_nothrow_swappable<T>::value;

	template<typename from, typename to>
	constexpr bool Is_convertible_v=Is_convertible<from, to>::value;

	template<typename T1,typename T2>
	constexpr bool Is_same_v = Is_same<T1, T2>::value;

	template<typename B,typename D>
	constexpr bool Is_base_of_v=detail::Is_base_of_<B, D>::value;
#endif
} //namespace TinySTL

#endif //TINYSTL_TYPE_TRAITS_H
