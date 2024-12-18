#pragma once

#include <bits/stdc++.h>
#include "fixed.h"

template <typename V_TYPE, size_t N, size_t M>
class VectorField {
public:
    static constexpr std::array<std::pair<int, int>, 4> deltas{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

    std::array<V_TYPE, deltas.size()> v[N][M];

    V_TYPE &add(int x, int y, int dx, int dy, V_TYPE dv) {
        return get(x, y, dx, dy) += dv;
    }

    V_TYPE &get(int x, int y, int dx, int dy) {
        size_t i = std::ranges::find(deltas, std::pair(dx, dy)) - deltas.begin();
        assert(i < deltas.size());
        return v[x][y][i];
    }
};
