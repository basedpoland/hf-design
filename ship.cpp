#include "ship.hpp"
#include "part-list.hpp"
#include "log.hpp"

namespace hf::design {

int ship::count(const part& x) const
{
    return parts[x.index].second;
}

ship::ship()
{
    add_part_(bridge);
}

void ship::add_part_(const part& x, int count, area_mode amode)
{
    ASSERT(count >= 0);
    if (amode && x.area() <= 0)
        ABORT("add_part_() wrong area for part %s", x.name);

    mass += x.mass * count;
    power += x.power * count;
    if (amode)
        area += count * x.area();
    cost += x.price * count;
    if (x.fuel >= 0)
        fuel += x.fuel * count;
    else
        fuel_flow -= x.fuel * count;
    thrust += x.thrust * count;
    if (x != e_d30s && x != e_rd51)
        horizontal_thrust += x.thrust * count;

    if (count)
    {
        //(void)find_part_or_die(x.name);
        parts[x.index].second += count;

        if (x == h_cor)
            sneaky_corners_left += count;
    }
}

void ship::add_part(const part& x, int count)
{
    ASSERT(count >= 0);
    add_part_(x, count);
    const auto& hull = part::find_hull(x);

    ASSERT(hull != null_part);
    if (hull != h_null)
        add_part_(hull, count, area_disabled);
}

decltype(ship::parts) ship::init_parts()
{
    const auto& all_parts = part::all_parts();
    decltype(ship::parts) parts{all_parts.size()};
    for (const auto* part : all_parts)
        parts.push_back({ part, 0 });
    return parts;
}

} // namespace hf::design
