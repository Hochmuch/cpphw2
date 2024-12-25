#pragma once

#include "fixed.h"
#include "fast_fixed.h"
#include "fluid.h"

#define ERROR_TYPES_MACRO \
static_assert(false,"Pass possible types using -DTYPES");
#ifdef TYPES
#define FIXED(N,K) Fixed<N,K>
#define FAST_FIXED(N,K) Fast_fixed<N,K>
#define DOUBLE double
#define FLOAT float

#ifdef SIZES
#endif
template<typename...>
struct Kit_of_types {
};

template<typename T>
struct Kit_of_types_SZ;

template<typename... Ts>
struct Kit_of_types_SZ<Kit_of_types<Ts...> > {
    static constexpr size_t value = sizeof...(Ts);
};

typedef Kit_of_types<TYPES> PrecalcedTypesVariety;


template<typename First, typename Second, typename Third>
struct Types_Kit {
};


template<typename TypeList_1, typename TypeList_2>
struct Merge;


template<typename... Kit_1, typename... Kit_2>
struct Merge<Kit_of_types<Kit_1...>, Kit_of_types<Kit_2...> > {
    using type = Kit_of_types<Kit_1..., Kit_2...>;
};

struct FluidConfig {
    static constexpr size_t WIDTH = 36;
    static constexpr size_t HEIGHT = 84;
};

template<typename... Tuples>
using fluid_variant = std::variant<
    Fluid<
        std::tuple_element_t<0, Tuples>,
        std::tuple_element_t<1, Tuples>,
        std::tuple_element_t<2, Tuples>,
        FluidConfig::WIDTH,
        FluidConfig::HEIGHT
    >...
>;

template<typename... First, typename... Second, typename... Third>
struct Types_Kit<Kit_of_types<First...>, Kit_of_types<Second...>, Kit_of_types<Third...> > {
    using type = Kit_of_types<std::tuple<First, Second, Third>...>;
};


template<typename First, typename Second, typename Third>
struct All_combo_maker;

template<typename... Firsts, typename... Seconds, typename... Thirds>
struct All_combo_maker<Kit_of_types<Firsts...>, Kit_of_types<Seconds...>, Kit_of_types<Thirds...> > {
private:
    template<typename TempFirst, typename TempSecond>
    struct make_combo_thirds {
        using type = Kit_of_types<std::tuple<TempFirst, TempSecond, Thirds>...>;
    };

    template<typename TempFirst>
    struct make_combo_seconds {
        template<typename...>
        struct Merger {
            using type = Kit_of_types<>;
        };

        template<typename TempSecond, typename... RemainingSecond>
        struct Merger<TempSecond, RemainingSecond...> {
            using type = typename Merge<
                typename make_combo_thirds<TempFirst, TempSecond>::type,
                typename Merger<RemainingSecond...>::type
            >::type;
        };

        using type = typename Merger<Seconds...>::type;
    };

    template<typename... >
    struct combine_all {
        using type = Kit_of_types<>;
    };

    template<typename First, typename... Remaining>
    struct combine_all<First, Remaining...> {
        using type = typename Merge<
            typename make_combo_seconds<First>::type,
            typename combine_all<Remaining...>::type
        >::type;
    };

public:
    using type = typename combine_all<Firsts...>::type;
};


template<typename var, typename tuple>
constexpr auto create_fluid() {
    return var(
        Fluid<std::tuple_element_t<0, tuple>, std::tuple_element_t<1, tuple>, std::tuple_element_t<2, tuple>,
            FluidConfig::WIDTH, FluidConfig::HEIGHT>());
}

template<typename... to_proc>
constexpr auto Packer(Kit_of_types<to_proc...>) {
    using variant = std::variant<fluid_variant<to_proc>...>;
    return std::array{variant(create_fluid<variant, to_proc>())...};
}

using types = All_combo_maker<PrecalcedTypesVariety, PrecalcedTypesVariety, PrecalcedTypesVariety>::type;
auto all_fluids = Packer(types{});
#else TYPES
ERROR_TYPES_MACRO
#endif
