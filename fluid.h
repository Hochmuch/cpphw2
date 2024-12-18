#pragma once

#include <bits/stdc++.h>
#include "fixed.h"
#include "fast_fixed.h"
#include "vectorfield.h"

template <typename P_TYPE, typename V_TYPE, typename V_FLOW_TYPE, size_t N, size_t M>
class Fluid {
public:
    //static constexpr size_t a = 32, b = 16;
    static constexpr size_t T = 1'000'000;
    static constexpr std::array<std::pair<int, int>, 4> deltas{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};

    char field[N][M + 1] = {
        "####################################################################################",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                       .........                                  #",
        "#..............#            #           .........                                  #",
        "#..............#            #           .........                                  #",
        "#..............#            #           .........                                  #",
        "#..............#            #                                                      #",
        "#..............#            #                                                      #",
        "#..............#            #                                                      #",
        "#..............#            #                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............################                     #                 #",
        "#...........................#....................................#                 #",
        "#...........................#....................................#                 #",
        "#...........................#....................................#                 #",
        "##################################################################                 #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "####################################################################################",
    };


    static constexpr P_TYPE inf = P_TYPE::from_raw(std::numeric_limits<int32_t>::max());

    static constexpr P_TYPE eps = P_TYPE::from_raw(deltas.size());

    P_TYPE rho[256];

    P_TYPE p[N][M], old_p[N][M];

    VectorField<V_TYPE, N, M> velocity{}, velocity_flow{};
    int last_use[N][M]{};
    int UT = 0;

    struct ParticleParams {
        char type;

        P_TYPE cur_p;

        std::array<P_TYPE, deltas.size()> v;

        void swap_with(int x, int y, Fluid &fluid) {
            std::swap(fluid.field[x][y], type);
            std::swap(fluid.p[x][y], cur_p);
            std::swap(fluid.velocity.v[x][y], v);
        }
    };

    std::mt19937 rnd{1337};

    std::tuple<V_FLOW_TYPE, bool, std::pair<int, int> > propagate_flow(int x, int y, V_FLOW_TYPE lim) {
        last_use[x][y] = UT - 1;
        V_FLOW_TYPE ret = 0;
        for (auto [dx, dy]: deltas) {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] != '#' && last_use[nx][ny] < UT) {
                auto cap = velocity.get(x, y, dx, dy);
                auto flow = velocity_flow.get(x, y, dx, dy);
                if (flow == cap) {
                    continue;
                }
                // assert(v >= velocity_flow.get(x, y, dx, dy));
                auto vp = std::min(lim, cap - flow);
                if (last_use[nx][ny] == UT - 1) {
                    velocity_flow.add(x, y, dx, dy, vp);
                    last_use[x][y] = UT;
                    // cerr << x << " " << y << " -> " << nx << " " << ny << " " << vp << " / " << lim << "\n";
                    return {vp, 1, {nx, ny}};
                }
                auto [t, prop, end] = propagate_flow(nx, ny, vp);
                ret += t;
                if (prop) {
                    velocity_flow.add(x, y, dx, dy, t);
                    last_use[x][y] = UT;
                    // cerr << x << " " << y << " -> " << nx << " " << ny << " " << t << " / " << lim << "\n";
                    return {t, prop && end != std::pair(x, y), end};
                }
            }
        }
        last_use[x][y] = UT;
        return {ret, 0, {0, 0}};
    }

    /*
    V_TYPE random01() {
        return V_TYPE::from_raw((rnd() & ((1 << b) - 1)));
    }
    */

    V_TYPE random01() {
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        return V_TYPE(distribution(rnd));
    }

    void propagate_stop(int x, int y, bool force = false) {
        if (!force) {
            bool stop = true;
            for (auto [dx, dy]: deltas) {
                int nx = x + dx, ny = y + dy;
                if (field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) > 0) {
                    stop = false;
                    break;
                }
            }
            if (!stop) {
                return;
            }
        }
        last_use[x][y] = UT;
        for (auto [dx, dy]: deltas) {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] == '#' || last_use[nx][ny] == UT || velocity.get(x, y, dx, dy) > 0) {
                continue;
            }
            propagate_stop(nx, ny);
        }
    }

    V_TYPE move_prob(int x, int y) {
        V_TYPE sum = 0;
        for (size_t i = 0; i < deltas.size(); ++i) {
            auto [dx, dy] = deltas[i];
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] == '#' || last_use[nx][ny] == UT) {
                continue;
            }
            auto v = velocity.get(x, y, dx, dy);
            if (v < 0) {
                continue;
            }
            sum += v;
        }
        return sum;
    }

    bool propagate_move(int x, int y, bool is_first) {
        last_use[x][y] = UT - is_first;
        bool ret = false;
        int nx = -1, ny = -1;
        do {
            std::array<V_TYPE, deltas.size()> tres;
            V_TYPE sum = 0;
            for (size_t i = 0; i < deltas.size(); ++i) {
                auto [dx, dy] = deltas[i];
                int nx = x + dx, ny = y + dy;
                if (field[nx][ny] == '#' || last_use[nx][ny] == UT) {
                    tres[i] = sum;
                    continue;
                }
                auto v = velocity.get(x, y, dx, dy);
                if (v < 0) {
                    tres[i] = sum;
                    continue;
                }
                sum += v;
                tres[i] = sum;
            }

            if (sum == 0) {
                break;
            }

            V_TYPE raand = random01();
            P_TYPE p = raand * sum;
            size_t d = std::ranges::upper_bound(tres, p) - tres.begin();

            auto [dx, dy] = deltas[d];
            nx = x + dx;
            ny = y + dy;
            assert(velocity.get(x, y, dx, dy) > 0 && field[nx][ny] != '#' && last_use[nx][ny] < UT);

            ret = (last_use[nx][ny] == UT - 1 || propagate_move(nx, ny, false));
        } while (!ret);
        last_use[x][y] = UT;
        for (size_t i = 0; i < deltas.size(); ++i) {
            auto [dx, dy] = deltas[i];
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) < 0) {
                propagate_stop(nx, ny);
            }
        }
        if (ret) {
            if (!is_first) {
                ParticleParams pp{};
                pp.swap_with(x, y, *this);
                pp.swap_with(nx, ny, *this);
                pp.swap_with(x, y, *this);
            }
        }
        return ret;
    }

    void run() {
        rho[' '] = 0.01;
        rho['.'] = 1000;
        P_TYPE g = 0.1;

        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                if (field[x][y] == '#')
                    continue;
                for (auto [dx, dy]: deltas) {
                    dirs[x][y] += (field[x + dx][y + dy] != '#');
                }
            }
        }

        for (size_t i = 0; i < T; ++i) {
            P_TYPE total_delta_p = 0;
            // Apply external forces
            for (size_t x = 0; x < N; ++x) {
                for (size_t y = 0; y < M; ++y) {
                    if (field[x][y] == '#')
                        continue;
                    if (field[x + 1][y] != '#')
                        velocity.add(x, y, 1, 0, g);
                }
            }

            // Apply forces from p
            memcpy(old_p, p, sizeof(p));
            for (size_t x = 0; x < N; ++x) {
                for (size_t y = 0; y < M; ++y) {
                    if (field[x][y] == '#')
                        continue;
                    for (auto [dx, dy]: deltas) {
                        int nx = x + dx, ny = y + dy;
                        if (field[nx][ny] != '#' && old_p[nx][ny] < old_p[x][y]) {
                            auto delta_p = old_p[x][y] - old_p[nx][ny];
                            auto force = delta_p;
                            auto &contr = velocity.get(nx, ny, -dx, -dy);
                            if (contr * rho[(int) field[nx][ny]] >= force) {
                                contr -= force / rho[(int) field[nx][ny]];
                                continue;
                            }
                            force -= contr * rho[(int) field[nx][ny]];
                            contr = 0;
                            velocity.add(x, y, dx, dy, force / rho[(int) field[x][y]]);
                            p[x][y] -= force / dirs[x][y]; // conv
                            total_delta_p -= force / dirs[x][y]; // conv
                        }
                    }
                }
            }

            // Make flow from velocities
            velocity_flow = {};
            bool prop = false;
            do {
                UT += 2;
                prop = 0;
                for (size_t x = 0; x < N; ++x) {
                    for (size_t y = 0; y < M; ++y) {
                        if (field[x][y] != '#' && last_use[x][y] != UT) {
                            auto [t, local_prop, _] = propagate_flow(x, y, 1);
                            if (t > 0) {
                                prop = 1;
                            }
                        }
                    }
                }
            } while (prop);

            // Recalculate p with kinetic energy
            for (size_t x = 0; x < N; ++x) {
                for (size_t y = 0; y < M; ++y) {
                    if (field[x][y] == '#')
                        continue;
                    for (auto [dx, dy]: deltas) {
                        auto old_v = velocity.get(x, y, dx, dy);
                        auto new_v = velocity_flow.get(x, y, dx, dy);
                        if (old_v > 0) {
                            assert(new_v <= old_v);
                            velocity.get(x, y, dx, dy) = new_v;
                            auto force = (old_v - new_v) * rho[(int) field[x][y]];
                            if (field[x][y] == '.')
                                force *= 0.8;
                            if (field[x + dx][y + dy] == '#') {
                                p[x][y] += force / dirs[x][y];
                                total_delta_p += force / dirs[x][y];
                            } else {
                                p[x + dx][y + dy] += force / dirs[x + dx][y + dy];
                                total_delta_p += force / dirs[x + dx][y + dy];
                            }
                        }
                    }
                }
            }

            UT += 2;
            prop = false;
            for (size_t x = 0; x < N; ++x) {
                for (size_t y = 0; y < M; ++y) {
                    if (field[x][y] != '#' && last_use[x][y] != UT) {
                        if (random01() < move_prob(x, y)) {
                            prop = true;
                            propagate_move(x, y, true);
                        } else {
                            propagate_stop(x, y, true);
                        }
                    }
                }
            }

            if (prop) {
                std::cout << "Tick " << i << ":\n";
                for (size_t x = 0; x < N; ++x) {
                    std::cout << field[x] << "\n";
                }
            }
        }
    }

    int dirs[N][M]{};
};
