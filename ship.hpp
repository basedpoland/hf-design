#pragma once

#include "part.hpp"
#include <vector>
#include <utility>

namespace hf::design {

struct part;

struct ship final
{
    enum area_mode : unsigned char { area_disabled = false, area_enabled = true };

    std::vector<std::pair<const part*, int>> parts = init_parts();
    float mass = 0, power = 0, fuel = 0, fuel_flow = 0, thrust = 0, horizontal_thrust = 0;
    int area = 0, cost = 0, sneaky_corners_left = 0;

    constexpr float twr() const { return thrust * 1000 / (mass * 9.81f); }
    constexpr float horizontal_twr() const { return horizontal_thrust * 1000 / (mass * 9.81f); }
    constexpr float combat_time() const { return fuel / fuel_flow; }
    constexpr float speed() const { return twr() * 90; }
    constexpr float fuel_usage() const { return 3600 * 20 * fuel_flow / speed();  }
    constexpr float range() const { return 50 * combat_time()/3600 * speed();  } // TODO inaccurate

    int count(const part& part) const;
    void add_part(const part& x, int count);
    void add_part_(const part& x, int count = 1, area_mode amode = area_enabled);
    static decltype(parts) init_parts();

    ship();
    ship& operator=(const ship&) = default;
};

} // namespace hf::design
