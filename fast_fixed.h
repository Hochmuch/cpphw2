#pragma once
#include <bits/stdc++.h>

template<size_t N, size_t K>
class Fast_fixed {
private:
public:
    static_assert(1 <= N && N <= 64, "неверный N");

    using StorageType = typename std::conditional_t<
        N <= 8, int_fast8_t,
        typename std::conditional_t<
            N <= 16, std::int_fast16_t,
            typename std::conditional_t<N <= 32, std::int_fast32_t, std::int_fast64_t> > >;

    StorageType value;


    constexpr Fast_fixed() : value(0) {
    }

    constexpr Fast_fixed(int v) : value(static_cast<StorageType>(v) << K) {
    }

    constexpr Fast_fixed(float f) : value(static_cast<StorageType>(f * (1 << K))) {
    }

    constexpr Fast_fixed(double f) : value(static_cast<StorageType>(f * (1 << K))) {
    }

    static constexpr Fast_fixed from_raw(StorageType x) {
        Fast_fixed ret;
        ret.value = x;
        return ret;
    }


    template<size_t N2, size_t K2>
    constexpr Fast_fixed &operator+=(const Fast_fixed<N2, K2> &other) {
        *this = *this + other;
        return *this;
    }

    template<size_t N2, size_t K2>
    constexpr Fast_fixed &operator-=(const Fast_fixed<N2, K2> &other) {
        *this = *this - other;
        return *this;
    }

    template<size_t N2, size_t K2>
    constexpr Fast_fixed &operator*=(const Fast_fixed<N2, K2> &other) {
        *this = *this * other;
        return *this;
    }

    template<size_t N2, size_t K2>
    constexpr Fast_fixed &operator/=(const Fast_fixed<N2, K2> &other) {
        *this = *this / other;
        return *this;
    }

    constexpr Fast_fixed operator-() const {
        return Fast_fixed::from_raw(-value);
    }

    constexpr double to_double() const {
        return static_cast<double>(value) / (1 << K);
    }
};


template<size_t N1, size_t K1, size_t N2, size_t K2>
constexpr auto operator+(const Fast_fixed<N1, K1> &a, const Fast_fixed<N2, K2> &b) {
    using ResultType = Fast_fixed<(N1 > N2 ? N1 : N2), (K1 > K2 ? K1 : K2)>;
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
constexpr auto operator-(const Fast_fixed<N1, K1> &lhs, const Fast_fixed<N2, K2> &rhs) {
    using ResultType = Fast_fixed<(N1 > N2 ? N1 : N2), (K1 > K2 ? K1 : K2)>;
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

template<size_t N1, size_t K1, size_t N2, size_t K2>
constexpr auto operator*(const Fast_fixed<N1, K1> &a, const Fast_fixed<N2, K2> &b) {
    constexpr size_t K = K1;
    using IntermediateType = int64_t;
    using ResultType = Fast_fixed<(N1 > N2 ? N1 : N2), K>;

    IntermediateType product = static_cast<IntermediateType>(a.value) * b.value;
    return ResultType::from_raw(static_cast<typename ResultType::StorageType>(product >> K1));
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr auto operator*(const Fast_fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        return Fast_fixed<N, K>::from_raw(a.value * b);
    } else {
        return Fast_fixed<N, K>(a.to_double() * static_cast<double>(b));
    }
}

template<size_t N1, size_t K1, size_t N2, size_t K2>
constexpr auto operator/(const Fast_fixed<N1, K1> &a, const Fast_fixed<N2, K2> &b) {
    constexpr size_t K = K1;
    using IntermediateType = int64_t;
    using ResultType = Fast_fixed<(N1 > N2 ? N1 : N2), K>;

    IntermediateType dividend = static_cast<IntermediateType>(a.value) << K;
    return ResultType::from_raw(static_cast<typename ResultType::StorageType>(dividend / b.value));
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr auto operator/(const Fast_fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        return Fast_fixed<N, K>::from_raw(a.value / b);
    } else {
        return Fast_fixed<N, K>(a.to_double() / static_cast<double>(b));
    }
}


template<size_t N1, size_t K1, size_t N2, size_t K2>
constexpr bool operator==(const Fast_fixed<N1, K1> &lhs, const Fast_fixed<N2, K2> &rhs) {
    using CommonType = Fast_fixed<(N1 > N2 ? N1 : N2), (K1 > K2 ? K1 : K2)>;
    return CommonType(lhs).value == CommonType(rhs).value;
}

template<size_t N1, size_t K1, size_t N2, size_t K2>
constexpr auto operator<=>(const Fast_fixed<N1, K1> &lhs, const Fast_fixed<N2, K2> &rhs) {
    using CommonType = Fast_fixed<(N1 > N2 ? N1 : N2), (K1 > K2 ? K1 : K2)>;
    return CommonType(lhs).value <=> CommonType(rhs).value;
}


template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr bool operator==(const Fast_fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        return a.value == (b << K);
    } else {
        return a.to_double() == static_cast<double>(b);
    }
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr bool operator!=(const Fast_fixed<N, K> &a, T b) {
    return !(a == b);
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr bool operator<(const Fast_fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        return a.value < (b << K);
    } else {
        return a.to_double() < static_cast<double>(b);
    }
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr bool operator<=(const Fast_fixed<N, K> &a, T b) {
    return (a < b) || (a == b);
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr bool operator>(const Fast_fixed<N, K> &a, T b) {
    return !(a <= b);
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr bool operator>=(const Fast_fixed<N, K> &a, T b) {
    return !(a < b);
}

template<size_t N, size_t K>
std::ostream &operator<<(std::ostream &out, const Fast_fixed<N, K> &fast_fixed) {
    out << static_cast<double>(fast_fixed.value) / (1 << K);
    return out;
}

template<size_t N, size_t K>
Fast_fixed<N, K> abs(const Fast_fixed<N, K> &x) {
    using CommonType = Fast_fixed<N, K>;
    return x.value < 0 ? -x : x;
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr Fast_fixed<N, K> &operator*=(Fast_fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        a.value *= b;
    } else {
        a.value = static_cast<int64_t>(a.to_double() * static_cast<double>(b) * (1 << K));
    }
    return a;
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr Fast_fixed<N, K> &operator/=(Fast_fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        a.value /= b;
    } else {
        a.value = static_cast<int64_t>(a.to_double() / static_cast<double>(b) * (1 << K));
    }
    return a;
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr Fast_fixed<N, K> &operator+=(Fast_fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        a.value += (b << K);
    } else {
        a.value += static_cast<int64_t>(b * (1 << K));
    }
    return a;
}

template<size_t N, size_t K, typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> > >
constexpr Fast_fixed<N, K> &operator-=(Fast_fixed<N, K> &a, T b) {
    if constexpr (std::is_integral_v<T>) {
        a.value -= (b << K);
    } else {
        a.value -= static_cast<int64_t>(b * (1 << K));
    }
    return a;
}
