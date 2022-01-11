#include "part.hpp"
#include "part-list.hpp"
#include "state.hpp"
#include "log.hpp"
#include "osdefs.hpp"
#include "cmdline.hpp"
#include <cstring>
#include <algorithm>

namespace hf::design {

static auto& static_parts()
{
    static std::vector<const part*> ret;
    return ret;
}

static bool part_lessp(const part* a, const part* b) { return strcmp(a->name, b->name) < 0; }
static bool part_name_lessp(const part* a, const char* b) { return strcmp(a->name, b) < 0; }

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
    auto it = std::lower_bound(parts.begin(), parts.end(), this, part_lessp);
    if (it != parts.end() && !strcmp(name, (**it).name))
        BUG("duplicate part -- '%s' - %s", name, (**it).name);
    parts.insert(it, this);
}

part::~part()
{
    auto& parts = static_parts();
    auto it = std::lower_bound(parts.begin(), parts.end(), this, part_lessp);
    if (it == parts.end() || !!strcmp(name, (**it).name))
        BUG("part not present in dtor -- '%s'", name);
    parts.erase(it);
}

const std::vector<const part*>& part::all_parts()
{
    return static_parts();
}

const part& maybe_part(const char* str)
{
    const auto& parts = part::all_parts();
    auto it = std::lower_bound(parts.cbegin(), parts.cend(), str, part_name_lessp);
    if (it != parts.cend() && !strcmp(str, (**it).name))
        return **it;
    else
        return null_part;
}

const part& part_or_die(const char* str)
{
    const part& ret = maybe_part(str);
    if (&ret == &null_part)
    {
        ERR("no such part -- '%s'", str);
        cmdline::terminate(EX_SOFTWARE);
    }
    return ret;
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
