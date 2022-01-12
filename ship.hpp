#pragma once

#include <unordered_map>

namespace hf::design {

struct part;

struct ship final
{
    enum area_mode : bool { area_disabled = false, area_enabled = true };

    float mass = 0, power = 0, fuel = 0, fuel_flow = 0, thrust = 0;
    int area = 0, cost = 0, sneaky_corners_left = 0;
    std::unordered_map<const part*, int> parts;

    constexpr float twr() const { return thrust * 100.f / mass + .1f; }
    constexpr float combat_time() const { return fuel / fuel_flow; }
    constexpr float speed() const { return twr() * 90; }
    constexpr float fuel_usage() const { return std::ceil(3600 * fuel_flow / speed() * 20);  }

    int count(const part& part) const;

    explicit ship();
    void add_part(const part& x, int count);
    void add_part_(const part& x, int count = 1, area_mode amode = area_enabled);
};

} // namespace hf::design
