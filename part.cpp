#include "part.hpp"
#include "part-list.hpp"
#include "state.hpp"
#include "log.hpp"
#include "osdefs.hpp"
#include "cmdline.hpp"

namespace hf::design {

unsigned djb2::operator()(const char* str) const
{
    unsigned hash = 5381;

    unsigned c;
    while ((c = (unsigned char)(*str++)) != '\0')
        hash = ((hash << 5) + hash) + c;

    return hash;
}

bool chars_equal::operator()(const char* a, const char* b) const
{
    for (; *a == *b && *a; a++, b++)
        (void)0;

    return !*a && !*b;
}


static auto& static_parts()
{
    static std::unordered_map<const char*, const part*, djb2, chars_equal> ret;
    return ret;
}

part::part(const char* name_, double mass_, double power_, part_size size_, int price_,
           float thrust_, double fuel_, int ammo_) :
    mass{(float)mass_},
    power{(float)power_},
    name{name_},
    size{size_},
    price{price_},
    fuel{(float)fuel_},
    thrust{(float)thrust_},
    ammo{ammo_}
{
    auto& parts = static_parts();
    if (parts.find(name) != parts.end())
        bug("duplicate part -- '%s'", name);
    parts[name_] = this;
}

part::~part()
{
    auto& parts = static_parts();
    auto cnt = parts.erase(name);
    if (!cnt)
        bug("part not present in dtor -- '%s'", name);
}

const std::unordered_map<const char*, const part*, djb2, chars_equal>& part::all_parts()
{
    return static_parts();
}

const part& maybe_part(const char* str)
{
    auto it = static_parts().find(str);
    if (it != std::end(static_parts()))
        return *it->second;
    else
        return null_part;
}

const part& part_or_die(const char* str)
{
    const part& ret = maybe_part(str);
    if (&ret == &null_part)
    {
        err("no such part -- '%s'", str);
        cmdline::terminate(EX_SOFTWARE);
    }
    return ret;
}

void add_part_(state& st, const part& x, int count, area_mode amode)
{
    st.mass += x.mass * count;
    st.power += x.power * count;
    if (amode == area_mode::enabled && x.size == sz_nan)
        bug("add_part_() wrong area sz_nan for part %s", x.name);
    if (amode == area_mode::enabled)
        st.area += count * std::abs(x.size);
    st.cost += x.price * count;
    if (x.fuel >= 0)
        st.fuel += x.fuel * count;
    else
        st.fuel_flow -= x.fuel * count;
    st.thrust += x.thrust * count;

    if (count)
    {
        //part_or_die(x.name);
        auto [it, b] = st.parts.emplace(&x, 0);
        it->second += count;

        if (x == h_cor)
            st.sneaky_corners_left += count;
    }
}

void add_part(state& st, const part& x, int count)
{
    add_part_(st, x, count);
    const part& hull = part_to_hull(x);

    if (hull == null_part)
        std::abort();
    if (hull != h_null)
        add_part_(st, hull, count, area_mode::disabled);
}

const part& part_to_hull(const part& x)
{
    switch (x.size)
    {
    case sz_1x1: return h_1x1;
    case sz_1x2: return h_1x2;
    case sz_2x2: return h_2x2;
    case sz_cor: return h_cor;
    case sz_bigfuel:
        return h_null;
    default: return null_part;
    }
}

} // namespace hf::design
