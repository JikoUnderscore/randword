#pragma once
// NOLINTBEGIN
#include <stdint.h>
#include <utility>

#define NODIS [[nodiscard]] 
#define cast(T, expr) static_cast<T>((expr))


// #define let const auto
// #define not !
// #define self (*this)

// (condition) ? true-clause : false-clause
#define TERNARY(condition, true_value, false_value) ((condition) ? (true_value) : (false_value))

using u8 = uint8_t;
using i8 = int8_t;
using u16 = uint16_t;
using i16 = int16_t;
using u32 = uint32_t;
using i32 = int32_t;
using u64 = uint64_t;
using i64 = int64_t;


#if defined _M_IX86 && _M_IX86_FP < 2 && !defined _M_FP_FAST
    typedef long double f32;
    typedef long double f64;
#else
    typedef float  f32;
    typedef double f64;
#endif


using usize = size_t;
using isize = ptrdiff_t;

// define DEF_COPY_MOVE(TypeName)                                                                                                                                                    \
//    TypeName(TypeName const&) noexcept = default;                                                                                                                                  \
//    TypeName& operator=(TypeName const&) noexcept = default;                                                                                                                       \
//    TypeName(TypeName&&) noexcept = default;                                                                                                                                       \
//    TypeName& operator=(TypeName&&) noexcept = default;


// #define DEL_COPY_MOVE(TypeName)                                                                                                                                                    \
//     TypeName(TypeName const&) noexcept = delete;                                                                                                                                  \
//     TypeName& operator=(TypeName const&) noexcept = delete;                                                                                                                             \
//     TypeName(TypeName&&) noexcept = delete;                                                                                                                                       \
//     TypeName& operator=(TypeName&&) noexcept = delete;



// #ifdef _DEBUG
// #include <type_traits>
// // template <class T>
// // [[nodiscard]] constexpr decltype(auto) jk_move(T&& arg) noexcept -> std::remove_reference_t<T>&& {
// //     static_assert(!std::is_const<std::remove_reference_t<T>>::value, "do not use std::move with a const T");
// //     // static_assert(std::is_const<T>::value, "do not use std::move with a const T");
// //     return std::move(arg);
// // }
// template<typename T>
// [[nodiscard]] constexpr decltype(auto) jk_move(T&& x) {
//     using V = std::remove_reference_t<T>;
//     static_assert(!std::is_const_v<V>, "Moving a Const Value");
//     return static_cast<V&&>(x);
// }
// #elif !_DEBUG
// // #define jk_move  std::move
// #endif


// template <typename T, typename VariantType = <TYPE_GOES_HERE>, std::size_t index = 0>
// comptime_fn_ variant_index() noexcept -> std::size_t {
//     static_assert(std::variant_size_v<VariantType> > index, "Type not found in variant");
//     if constexpr (index == std::variant_size_v<VariantType>) {
//         return index;
//     }
//     else if constexpr (std::is_same_v<std::variant_alternative_t<index, VariantType>, T>) {
//         return index;
//     }
//     else {
//         return variant_index<T, VariantType, index + 1>();
//     }
// }


template <typename F>
struct gbprivDefer {
    F f;
    constexpr explicit gbprivDefer(F&& fun) : f(std::forward<F>(fun)) {
    }
    ~gbprivDefer() {
        f();
    }
    gbprivDefer(gbprivDefer const&) noexcept = delete;
    gbprivDefer& operator=(gbprivDefer const&) noexcept = delete;
};

template <typename F>
constexpr auto gb_defer_func_(F&& f) -> gbprivDefer<F> {
    return gbprivDefer<F>(std::forward<F>(f));
}

#define GB_DEFER_1(x, y) x##y
#define GB_DEFER_2(x, y) GB_DEFER_1(x, y)
#define GB_DEFER_3(x) GB_DEFER_2(x, __COUNTER__)
#define defer(code) auto GB_DEFER_3(_defer_) = gb_defer_func_([&]() -> void { (code); })
// NOLINTEND
