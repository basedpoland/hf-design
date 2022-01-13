#undef NDEBUG
#include "part.hpp"
#include "part-list.hpp"
#include "ship.hpp"
#include "cmdline.hpp"
#include "osdefs.hpp"
#include "log.hpp"

#include "getopt.h"
#include <cassert>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <algorithm>

namespace hf::design {

void report_pretty(const ship& st, cmdline::fmt format);
void report_csv(const ship& st, int k);

static bool add_gun(ship& st, const char* str)
{
    char buf[128 + 2] = { 'g', '_', '\0' };
    if (strlen(str) >= sizeof(buf))
        return false;
    int count = 0;
    int ret = sscanf(str, "%d:%127s", &count, buf+2);
    buf[sizeof(buf)-1] = '\0';
    if (ret != 2 || count <= 0)
    {
        ERR("wrong gun specification -- '%s'", str);
        return false;
    }

    const auto& p = part::find_part(buf);
    if (p == null_part)
    {
        ERR("no such gun -- '%s'", buf + 2);
        return false;
    }
    if (p.ammo >= 0)
    {
        ERR("part not a gun -- '%s'", str);
        return false;
    }
    st.add_part(p, count);
    int ammo = -p.ammo * count;
    int ammo_big = ammo / 2, ammo_small = ammo % 2;
    st.add_part(ammo_2x2, ammo_big);
    st.add_part(ammo_1x2, ammo_small);

    return true;
}

static void add_fixed(ship& st, const cmdline& params)
{
    constexpr int min_for_single_piece = 4;

    if (params.fixed_engine_count < min_for_single_piece ||
        params.fixed_engine_count % 2 != 0)
    {
        st.add_part(chassis_2, 2);
        st.add_part_(chassis_1, 2, ship::area_disabled);
    }
    else
    {
        st.add_part(e_d30s, params.fixed_engine_count);
        st.add_part(chassis_2, 1); // gear connected to corner piece
        st.add_part_(chassis_2, 5, ship::area_disabled); // connected to other gear
        st.add_part_(chassis_1, 2, ship::area_disabled); // small legs for landing stability
    }
}

static bool add_fuel(ship& st, const cmdline& params)
{
    assert(st.fuel_flow > 1e-6f);
    int num_tanks = (int)std::ceil(st.fuel_flow * params.combat_time / tank_1x2.fuel);
    if (params.use_big_tanks)
    {
        float ratio = tank_4x4.fuel / tank_1x2.fuel;
        int num = (int)((std::max(0, num_tanks - st.sneaky_corners_left)) / ratio); // num_tanks / 11.25
        if (!num)
            return false;
        num_tanks -= (int)(num * ratio);
        assert(num_tanks >= 0);
        st.add_part_(tank_4x4, num);
    }
    int sneaky_tanks = std::min(st.sneaky_corners_left / 2, num_tanks); // use the cornerless 2x2 pieces to stick in extra tanks
    num_tanks -= sneaky_tanks;
    st.sneaky_corners_left -= sneaky_tanks*2;
    assert(sneaky_tanks >= 0); assert(num_tanks >= 0);
    st.add_part(tank_1x2, num_tanks);
    st.add_part_(tank_1x2, sneaky_tanks, ship::area_disabled);
    st.add_part_(h_05, sneaky_tanks*2, ship::area_disabled);
    st.add_part(fire, params.num_extinguishers);

    assert(st.fuel > 0);

    return true;
}

static void add_power(ship& st)
{
    float power = -st.power;
    assert(power > 1e-6f);
    float x = std::fmod(power, pwr_2x2.power);
    if (x <= 2*pwr_1x2.power) // they weigh less than the full generator
    {
        int small_gens = x > pwr_1x2.power ? 2 : 1;
        st.add_part(pwr_1x2, small_gens);
        power = std::max(0.f, power - pwr_1x2.power*small_gens);
    }
    int big_gens = (int)std::ceil((power + 1e-6f) / pwr_2x2.power);
    st.add_part(pwr_2x2, big_gens);
}

static void add_armor(ship& st, const cmdline& params)
{
    if (params.armor_layers < 1e-6f)
        return;

    float circumference = std::sqrt((float)st.area) * 4;
    const part* static_engines[] = { &e_d30s };
    for (const auto* part : static_engines)
    {
        int sz = std::abs(part->area());
        assert(sz >= 1);
        circumference -= std::sqrt((float)sz) / 4;
    }
    assert(circumference > 0);
    int num_armor = (int)std::ceil(circumference);
    st.add_part(arm_1x1, num_armor);
}

static bool filter_ship(const ship& st, const cmdline& params)
{
    bool ret = true;
    ret &= params.twr.check(st.twr());
    ret &= params.cost.check(st.cost);
    ret &= params.fuel_usage.check(st.fuel_usage());

    return ret;
}

static void do_search(const ship& st_, const cmdline& params, int& num_designs)
{
    // there are only two vectoring engine types, so simply:
    for (int i = std::max(1, params.engines.min); i <= params.engines.max; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            int num_d30 = j, num_nk25 = i-j;
            ship st = st_;
            st.add_part(e_d30, num_d30);
            st.add_part(e_nk25, num_nk25);
            if (!add_fuel(st, params))
                continue;
            add_power(st);
            add_armor(st, params);

            if (!filter_ship(st, params))
                continue;

            if (params.format == params.fmt_csv)
                report_csv(st, num_designs);
            else
                report_pretty(st, params.format);

            if (++num_designs >= params.num_matches)
                return;
        }
    }

    if (num_designs == 0)
        WARN("no designs could be generated within the constraints.");
}

extern "C" int main(int argc, char** argv)
{
#ifdef _WIN32
    if (const char* c = strrchr(argv[0], '.'); c && *c)
        argv[0][c - argv[0]] = '\0';
    argv[0] = std::max(argv[0], strrchr(argv[0], '\\')+1);
#endif
    argv[0] = std::max(argv[0], strrchr(argv[0], '/')+1);

    try {
        if (argc < 2)
            cmdline::usage(argv[0]);

        ship st;
        auto params = cmdline::parse_options(argc, argv);
        add_fixed(st, params);
        if (musl_optind == argc)
            cmdline::usage(argv[0]);
        for (int i = musl_optind; i < argc; i++)
            if (!add_gun(st, argv[i]))
            {
                INFO("Try '%s -G' to list supported guns.", params.argv[0]);
                params.terminate(EX_USAGE);
            }
        int nresults = 0;
        do_search(st, params, nresults);
        if (params.use_big_tanks)
        {
            params.use_big_tanks = false;
            do_search(st, params, nresults);
        }
        return 0;
    } catch (const exit_status& x) {
        return x.code;
    }
}

} // namespace hf::design
