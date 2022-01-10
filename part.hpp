#pragma once
#include <cstddef>
#include <unordered_map>

namespace hf::design {

enum part_size : int { sz_1x1 = 1, sz_2x2 = 4, sz_1x2 = 2, sz_bigfuel = 16, sz_cor = -4, sz_nan = -1};

struct djb2 final { unsigned operator()(const char* str) const; };
struct chars_equal { bool operator()(const char* a, const char* b) const; };

struct part final
{
    static const std::unordered_map<const char*, const part*, djb2, chars_equal>& all_parts();

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

enum class area_mode : bool { disabled = false, enabled = true };

const part& maybe_part(const char* str);
const part& part_or_die(const char* str);

void add_part_(state& st, const part& x, int count = 1, area_mode amode = area_mode::enabled);
void add_part(state& st, const part& x, int count = 1);
const part& part_to_hull(const part& x);

} // namespace hf::design
