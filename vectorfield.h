#pragma once

#include <bits/stdc++.h>
#include "fixed.h"

template <typename TYPE, size_t N, size_t M>
class VectorField {
public:
    static constexpr std::array<std::pair<int, int>, 4> deltas{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

    std::array<TYPE, deltas.size()> v[N][M];

    TYPE &add(int x, int y, int dx, int dy, TYPE dv) {
        return get(x, y, dx, dy) += dv;
    }

    TYPE &get(int x, int y, int dx, int dy) {
        size_t i = std::ranges::find(deltas, std::pair(dx, dy)) - deltas.begin();
        assert(i < deltas.size());
        return v[x][y][i];
    }
};
