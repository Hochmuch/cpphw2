#pragma once

#include <bits/stdc++.h>

template <size_t N, size_t M>
inline void read(char (&x)[N][M]) {
    std::ifstream fin("input.txt");
    std::string line;

    int i = 0;
    while (std::getline(fin, line)) {
        for (int k = 0; k < M; ++k) {
            x[i][k] = line[k];
        }
        i++;
    }
    fin.close();
}