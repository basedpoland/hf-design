#pragma once

#include <unordered_map>

namespace hf::design {

struct part;

struct ship final
{
    enum area_mode : unsigned char { area_disabled = false, area_enabled = true };

    float mass = 0, power = 0, fuel = 0, fuel_flow = 0, thrust = 0, horizontal_thrust = 0;
    int area = 0, cost = 0, sneaky_corners_left = 0;
    std::unordered_map<const part*, int> parts;

    constexpr float twr() const { return thrust * 100.f / mass; }
    constexpr float horizontal_twr() const { return horizontal_thrust * 100.f / mass; }
    constexpr float combat_time() const { return fuel / fuel_flow; }
    constexpr float speed() const { return twr() * 90; }
    constexpr float fuel_usage() const { return 3600 * 20 * fuel_flow / speed();  } // note: fuel usage is 20 times lower outside combat
    //constexpr float range() const { return 1e3f * fuel / fuel_usage(); } // TODO inaccurate

    int count(const part& part) const;

    explicit ship();
    void add_part(const part& x, int count);
    void add_part_(const part& x, int count = 1, area_mode amode = area_enabled);
};

} // namespace hf::design
