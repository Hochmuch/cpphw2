#pragma once

#include <bits/stdc++.h>
#include "basic_fixed.h"

template<size_t N, size_t K>
class Fixed {
private:
public:
    static_assert(N == 8 || N == 16 || N == 32 || N == 64, "неверный N");

    using StorageType = typename std::conditional_t<
        N <= 8, std::int8_t,
        typename std::conditional_t<
            N <= 16, std::int16_t,
            typename std::conditional_t<N <= 32, std::int32_t, std::int64_t> > >;

    StorageType value;

    constexpr Fixed() : value(0) {
    }

    constexpr Fixed(int v) : value(static_cast<StorageType>(v) << K) {
    }

    constexpr Fixed(float f) : value(static_cast<StorageType>(f * (1 << K))) {
    }

    constexpr Fixed(double f) : value(static_cast<StorageType>(f * (1 << K))) {
    }

    template<size_t N2, size_t K2>
    constexpr Fixed(const Fast_fixed<N2, K2> &other) {
        if constexpr (K > K2) {
            value = static_cast<StorageType>(other.value) << (K - K2);
        } else if constexpr (K < K2) {
            value = static_cast<StorageType>(other.value) >> (K2 - K);
        } else {
            value = static_cast<StorageType>(other.value);
        }
    }



    static constexpr Fixed from_raw(StorageType x) {
        Fixed ret;
        ret.value = x;
        return ret;
    }

    template<size_t N2, size_t K2>
    constexpr Fixed &operator+=(const Fixed<N2, K2> &other) {
        *this = *this + other;
        return *this;
    }

    template<size_t N2, size_t K2>
    constexpr Fixed &operator-=(const Fixed<N2, K2> &other) {
        *this = *this - other;
        return *this;
    }

    template<size_t N2, size_t K2>
    constexpr Fixed &operator*=(const Fixed<N2, K2> &other) {
        *this = *this * other;
        return *this;
    }

    template<size_t N2, size_t K2>
    constexpr Fixed &operator/=(const Fixed<N2, K2> &other) {
        *this = *this / other;
        return *this;
    }

    constexpr Fixed operator-() const {
        return Fixed::from_raw(-value);
    }

    constexpr double to_double() const {
        return static_cast<double>(value) / (1 << K);
    }

    constexpr explicit operator float() const {
        return static_cast<float>(value) / (1 << K);
    }

    constexpr explicit operator double() const {
        return static_cast<double>(value) / (1 << K);
    }
};

template<size_t N1, size_t K1, size_t N2, size_t K2>
constexpr auto operator+(const Fixed<N1, K1> &a, const Fixed<N2, K2> &b) {
    using ResultType = Fixed<(N1 > N2 ? N1 : N2), (K1 > K2 ? K1 : K2)>;
    using StorageType = typename ResultType::StorageType;

    StorageType a_value = a.value;
    StorageType b_value = b.value;

    if constexpr (K1 > K2) {
        b_value <<= (K1 - K2);
    } else if constexpr (K2 > K1) {
        a_value <<= (K2 - K1);
    }

    return ResultType::from_raw(a_value + b_value);
}

template<size_t N1, size_t K1, size_t N2, size_t K2>
constexpr auto operator-(const Fixed<N1, K1> &lhs, const Fixed<N2, K2> &rhs) {
    using ResultType = Fixed<(N1 > N2 ? N1 : N2), (K1 > K2 ? K1 : K2)>;
    using StorageType = typename ResultType::StorageType;

    StorageType lhs_value = lhs.value;
    StorageType rhs_value = rhs.value;

    if constexpr (K1 > K2) {
        rhs_value <<= (K1 - K2);
    } else if constexpr (K2 > K1) {
        lhs_value <<= (K2 - K1);
    }

    return ResultType::from_raw(lhs_value - rhs_value);
}

// операции с fixed

template<size_t N1, size_t K1, size_t N2, size_t K2>
constexpr auto operator*(const Fixed<N1, K1> &a, const Fixed<N2, K2> &b) {
    constexpr size_t K = K1;
    using IntermediateType = int64_t;
    using ResultType = Fixed<(N1 > N2 ? N1 : N2), K>;

    IntermediateType product = static_cast<IntermediateType>(a.value) * b.value;
    return ResultType::from_raw(static_cast<typename ResultType::StorageType>(product >> K1));
}

template<size_t N1, size_t K1, size_t N2, size_t K2>
constexpr auto operator/(const Fixed<N1, K1> &a, const Fixed<N2, K2> &b) {
    constexpr size_t K = K1;
    using IntermediateType = int64_t;
    using ResultType = Fixed<(N1 > N2 ? N1 : N2), K>;

    IntermediateType dividend = static_cast<IntermediateType>(a.value) << K;
    return ResultType::from_raw(static_cast<typename ResultType::StorageType>(dividend / b.value));
}

template<size_t N1, size_t K1, size_t N2, size_t K2>
constexpr bool operator==(const Fixed<N1, K1> &lhs, const Fixed<N2, K2> &rhs) {
    using CommonType = Fixed<(N1 > N2 ? N1 : N2), (K1 > K2 ? K1 : K2)>;
    return CommonType(lhs).value == CommonType(rhs).value;
}

template<size_t N1, size_t K1, size_t N2, size_t K2>
constexpr auto operator<=>(const Fixed<N1, K1> &lhs, const Fixed<N2, K2> &rhs) {
    using CommonType = Fixed<(N1 > N2 ? N1 : N2), (K1 > K2 ? K1 : K2)>;
    return CommonType(lhs).value <=> CommonType(rhs).value;
}

template<size_t N, size_t K>
std::ostream &operator<<(std::ostream &out, const Fixed<N, K> &fixed) {
    out << static_cast<double>(fixed.value) / (1 << K);
    return out;
}

template<size_t N, size_t K>
Fixed<N, K> abs(const Fixed<N, K> &x) {
    using CommonType = Fixed<N, K>;
    return x.value < 0 ? -x : x;
}


// операции с арифметическими типами справа

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr auto operator+(const Fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        return Fixed<N, K>::from_raw(a.value + b);
    } else {
        return Fixed<N, K>(a.to_double() + static_cast<double>(b));
    }
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr auto operator-(const Fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        return Fixed<N, K>::from_raw(a.value - b);
    } else {
        return Fixed<N, K>(a.to_double() - static_cast<double>(b));
    }
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr auto operator*(const Fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        return Fixed<N, K>::from_raw(a.value * b);
    } else {
        return Fixed<N, K>(a.to_double() * static_cast<double>(b));
    }
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr auto operator/(const Fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        return Fixed<N, K>::from_raw(a.value / b);
    } else {
        return Fixed<N, K>(a.to_double() / static_cast<double>(b));
    }
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr bool operator==(const Fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        return a.value == (b << K);
    } else {
        return a.to_double() == static_cast<double>(b);
    }
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr bool operator!=(const Fixed<N, K> &a, T b) {
    return !(a == b);
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr bool operator<(const Fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        return a.value < (b << K);
    } else {
        return a.to_double() < static_cast<double>(b);
    }
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr bool operator<=(const Fixed<N, K> &a, T b) {
    return (a < b) || (a == b);
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr bool operator>(const Fixed<N, K> &a, T b) {
    return !(a <= b);
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr bool operator>=(const Fixed<N, K> &a, T b) {
    return !(a < b);
}


template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr Fixed<N, K> &operator*=(Fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        a.value *= b;
    } else {
        a.value = static_cast<int64_t>(a.to_double() * static_cast<double>(b) * (1 << K));
    }
    return a;
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr Fixed<N, K> &operator/=(Fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        a.value /= b;
    } else {
        a.value = static_cast<int64_t>(a.to_double() / static_cast<double>(b) * (1 << K));
    }
    return a;
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr Fixed<N, K> &operator+=(Fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        a.value += (b << K);
    } else {
        a.value += static_cast<int64_t>(b * (1 << K));
    }
    return a;
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr Fixed<N, K> &operator-=(Fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        a.value -= (b << K);
    } else {
        a.value -= static_cast<int64_t>(b * (1 << K));
    }
    return a;
}

// ОПЕРАЦИИ С FAST_FIXED

template<size_t N1, size_t K1, size_t N2, size_t K2>
constexpr auto operator+(const Fixed<N1, K1> &a, const Fast_fixed<N2, K2> &b) {
    using ResultType = Fixed<(N1 > N2 ? N1 : N2), (K1 > K2 ? K1 : K2)>;
    using StorageType = typename ResultType::StorageType;

    StorageType a_value = a.value;
    StorageType b_value = b.value;

    if constexpr (K1 > K2) {
        b_value <<= (K1 - K2);
    } else if constexpr (K2 > K1) {
        a_value <<= (K2 - K1);
    }

    return ResultType::from_raw(a_value + b_value);
}

template<size_t N1, size_t K1, size_t N2, size_t K2>
constexpr auto operator-(const Fixed<N1, K1> &a, const Fast_fixed<N2, K2> &b) {
    using ResultType = Fixed<(N1 > N2 ? N1 : N2), (K1 > K2 ? K1 : K2)>;
    using StorageType = typename ResultType::StorageType;

    StorageType a_value = a.value;
    StorageType b_value = b.value;

    if constexpr (K1 > K2) {
        b_value <<= (K1 - K2);
    } else if constexpr (K2 > K1) {
        a_value <<= (K2 - K1);
    }

    return ResultType::from_raw(a_value - b_value);
}

template<size_t N1, size_t K1, size_t N2, size_t K2>
constexpr auto operator*(const Fixed<N1, K1> &a, const Fast_fixed<N2, K2> &b) {
    constexpr size_t K = K1;
    using IntermediateType = int64_t;
    using ResultType = Fixed<(N1 > N2 ? N1 : N2), K>;

    IntermediateType product = static_cast<IntermediateType>(a.value) * b.value;
    return ResultType::from_raw(static_cast<typename ResultType::StorageType>(product >> K1));
}

template<size_t N1, size_t K1, size_t N2, size_t K2>
constexpr auto operator/(const Fixed<N1, K1> &a, const Fast_fixed<N2, K2> &b) {
    constexpr size_t K = K1;
    using IntermediateType = int64_t;
    using ResultType = Fixed<(N1 > N2 ? N1 : N2), K>;

    IntermediateType dividend = static_cast<IntermediateType>(a.value) << K;
    return ResultType::from_raw(static_cast<typename ResultType::StorageType>(dividend / b.value));
}

// Операции с арифметическими типами слева

template<typename T, size_t N, size_t K, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
constexpr auto operator-(T a, const Fixed<N, K>& b) {
    return Fixed<N, K>(a) - b;
}

template<typename T, size_t N, size_t K, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
constexpr auto operator+(T a, const Fixed<N, K>& b) {
    return Fixed<N, K>(a) + b;
}

template<typename T, size_t N, size_t K, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
constexpr auto operator*(T a, const Fixed<N, K>& b) {
    return Fixed<N, K>(a) * b;
}

template<typename T, size_t N, size_t K, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
constexpr auto operator/(T a, const Fixed<N, K>& b) {
    return Fixed<N, K>(a) / b;
}

template<typename T, size_t N, size_t K, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T& operator-=(T& a, const Fixed<N, K>& b) {
    a -= b.to_double();
    return a;
}

template<typename T, size_t N, size_t K, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T& operator+=(T& a, const Fixed<N, K>& b) {
    a += b.to_double();
    return a;
}

template<typename T, size_t N, size_t K, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T& operator*=(T& a, const Fixed<N, K>& b) {
    a *= b.to_double();
    return a;
}

template<typename T, size_t N, size_t K, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
T& operator/=(T& a, const Fixed<N, K>& b) {
    a /= b.to_double();
    return a;
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr bool operator<(T a, const Fixed<N, K> &b) {
    return (static_cast<Fixed<N, K>>(a) < b);
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr bool operator==(T a, const Fixed<N, K> &b) {
    return (static_cast<Fixed<N, K>>(a) == b);
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr bool operator<=(T a, const Fixed<N, K> &b) {
    return (a < b) || (a == b);
}