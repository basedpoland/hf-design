#pragma once
#include <vector>

namespace hf::design {

enum part_size : int { sz_1x1 = 1, sz_2x2 = 4, sz_1x2 = 2, sz_bigfuel = 16, sz_cor = -4, sz_nan = -1};

struct part final
{
    static const std::vector<const part*>& all_parts();

    float mass, power;
    const char* name = nullptr;
    part_size size = sz_nan;
    int price;
    float fuel, thrust;
    int ammo;

    part() = delete;

    part(const char* name, double mass, double power, part_size size, int price,
         float thrust = 0, double fuel = 0, int ammo = 0);
    ~part();

    part(const part&&) = delete;
    part(part&&) = delete;
    part& operator==(const part&) = delete;
    part& operator==(part&&) = delete;
};

struct state;

constexpr inline bool operator==(const part& a, const part& b) { return &a == &b; }
constexpr inline bool operator!=(const part& a, const part& b) { return !(a == b); }

} // namespace hf::design
