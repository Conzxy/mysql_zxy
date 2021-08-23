#ifndef TINYSTL_STL_TYPE_TRAITS_BASE_H
#define TINYSTL_STL_TYPE_TRAITS_BASE_H

//helper of _type_traits<>
#define DEFINE_TYPE_TRAITS(TYPE)                            \
template<>                                                  \
struct _type_traits<TYPE> {                                 \
    using has_trivially_default_constructor  =_true_type;   \
    using has_trivially_copy_constructor     =_true_type;   \
    using has_trivially_assignment_operator  =_true_type;   \
    using has_trivially_destructor           =_true_type;   \
    using is_POD_type                        =_true_type;   \
};                                                          \


namespace TinySTL{
    
    template<typename T,T val>
    struct Integral_constant{
        static constexpr T value=val;
        using value_type=T;
        using type=Integral_constant;
        constexpr operator value_type()const noexcept{
            return value;
        }
        constexpr value_type operator()()const noexcept{
            return value;
        }
    };

    template<bool val>
    using Bool_constant=Integral_constant<bool,val>;

    using _true_type=Bool_constant<true>;
    using _false_type=Bool_constant<false>;


    template<typename T>
    struct _type_traits{
        using has_trivially_default_constructor     =_false_type;   
        using has_trivially_copy_constructor        =_false_type;   
        using has_trivially_assignment_operator     =_false_type;   
        using has_trivially_destructor              =_false_type;   
        using is_POD_type                           =_false_type;   
    };

    DEFINE_TYPE_TRAITS(char)
    DEFINE_TYPE_TRAITS(signed char)
    DEFINE_TYPE_TRAITS(unsigned char)
    DEFINE_TYPE_TRAITS(char16_t)
    DEFINE_TYPE_TRAITS(char32_t)
    DEFINE_TYPE_TRAITS(bool)
    DEFINE_TYPE_TRAITS(wchar_t)
    DEFINE_TYPE_TRAITS(signed int)
    DEFINE_TYPE_TRAITS(unsigned int)
    DEFINE_TYPE_TRAITS(signed short)
    DEFINE_TYPE_TRAITS(unsigned short)
    DEFINE_TYPE_TRAITS(signed long)
    DEFINE_TYPE_TRAITS(unsigned long)
    DEFINE_TYPE_TRAITS(signed long long)
    DEFINE_TYPE_TRAITS(unsigned long long)

    DEFINE_TYPE_TRAITS(double)
    DEFINE_TYPE_TRAITS(float)
    DEFINE_TYPE_TRAITS(double long)
}
#endif //TINYSTL_STL_TYPE_TRAITS_BASE_H
