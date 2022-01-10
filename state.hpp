#pragma once

#include <unordered_map>

namespace hf::design {

struct part;

struct state final
{
    float mass = 0, power = 0, fuel = 0, fuel_flow = 0, thrust = 0;
    int area = 0, cost = 0, sneaky_corners_left = 0;
    std::unordered_map<const part*, int> parts;

    constexpr float twr() const { return thrust * 100.f / mass + .1f; }
    constexpr float combat_time() const { return fuel / fuel_flow; }
    constexpr float speed() const { return twr() * 90; }
    constexpr float fuel_usage() const { return 3600 * fuel_flow / speed() * 20;  }

    int count(const part& part) const;

    explicit state();
};

} // namespace hf::design
