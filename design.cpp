#include "part.hpp"
#include "part-list.hpp"
#include "ship.hpp"
#include "cmdline.hpp"
#include "defs.hpp"
#include "log.hpp"

#include "getopt.h"
#include <cmath>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <tuple>

namespace hf::design {

bool report_pretty(const ship& st, int k);
bool report_csv(const ship& st, int k);

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

static void add_legs(ship& st, const cmdline& params)
{
    constexpr int min_engines_for_single_leg = 4;

    auto [nlegs, chassis] = params.chassis;
    int total = 0;
    for (unsigned i = 0 ; i < std::size(chassis); i++)
        total += chassis[i];
    if (nlegs && !total)
    {
        ERR("invalid chassis specification");
        params.seek_help();
        terminate(EX_USAGE);
    }
    if (!nlegs && total)
        nlegs = 2;
    if (total)
    {
        const part* parts[] = { &leg1, &leg2, &leg3, &leg4 };
        st.add_part_(h_cor, nlegs, ship::area_disabled);
        for (unsigned i = 0; i < std::size(parts); i++)
            st.add_part_(*parts[i], chassis[i], ship::area_disabled);
    }
    else if (int n = st.count(e_d30s);
             st.count(e_rd51) || n % 2 != 0 || n < min_engines_for_single_leg)
    {
        st.add_part(leg2, 2);
        st.add_part_(leg1, 2, ship::area_disabled);
    }
    else
    {
        st.add_part(leg2, 1); // gear connected to corner piece
        st.add_part_(leg2, 6, ship::area_disabled); // connected to other gear
        st.add_part_(leg1, 2, ship::area_disabled); // small legs for landing stability
    }
}

static bool add_fuel(ship& st, const cmdline& params)
{
    ASSERT(st.fuel_flow > 1e-6f);
    int num_tanks = (int)std::ceil(st.fuel_flow * params.combat_time / tank_1x2.fuel);
    if (params.use_big_tanks)
    {
        float ratio = tank_4x4.fuel / tank_1x2.fuel;
        int num = (int)((std::max(0, num_tanks - st.sneaky_corners_left)) / ratio); // num_tanks / 11.25
        if (!num)
            return false;
        num_tanks -= (int)(num * ratio);
        ASSERT(num_tanks >= 0);
        st.add_part_(tank_4x4, num);
    }
    int sneaky_tanks = std::min(st.sneaky_corners_left / 2, num_tanks); // use the cornerless 2x2 pieces to stick in extra tanks
    num_tanks -= sneaky_tanks;
    st.sneaky_corners_left -= sneaky_tanks*2;
    ASSERT(sneaky_tanks >= 0); ASSERT(num_tanks >= 0); ASSERT(st.sneaky_corners_left >= 0);
    st.add_part(tank_1x2, num_tanks);
    st.add_part_(tank_1x2, sneaky_tanks, ship::area_disabled);
    st.add_part_(h_05, sneaky_tanks*2, ship::area_disabled);
    st.add_part(fire, params.num_extinguishers);

    ASSERT(st.fuel > 0);

    return true;
}

static void add_power(ship& st, const cmdline& params)
{
    float power = -st.power * params.power;
    ASSERT(power > 1e-6f);
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
        ASSERT(sz >= 1);
        circumference -= std::sqrt((float)sz) / 2;
    }
    ASSERT(circumference > 0);
    int num_armor = (int)std::ceil(circumference*params.armor_layers);
    st.add_part(arm_1x1, num_armor);
}

static bool filter_ship(const ship& st, const cmdline& params)
{
    switch (int N = st.count(e_d30) + st.count(e_nk25); params.engine_parity)
    {
    using parity = cmdline::parity;
    case parity::any: break;
    case parity::even: if (N % 2 != 0) return false; break;
    case parity::odd:  if (N % 2 == 0) return false; break;
    }

    return params.twr.check(st.twr()) &&
           params.cost.check(st.cost) &&
           params.fuel_usage.check(st.fuel_usage()) &&
           params.horizontal_twr.check(st.horizontal_twr());
}

static void do_search1(const ship& st_, ship& st, const cmdline& params, const std::tuple<int, int, int, int, int>& n, int& num_designs)
{
    auto [num_d30s, num_rd51, num_d30, num_nk25, num_rd59] = n;

    st = st_;
    st.mass += params.extra_mass;
    st.power -= params.extra_power;
    st.add_part(e_d30s, num_d30s);
    st.add_part(e_rd51, num_rd51);
    st.add_part(e_d30, num_d30);
    st.add_part(e_nk25, num_nk25);
    st.add_part(e_rd59, num_rd59);
    add_legs(st, params);
    if (!add_fuel(st, params))
        return;
    add_power(st, params);
    add_armor(st, params);

    if (!filter_ship(st, params))
        return;

    switch (params.format)
    {
    case cmdline::fmt_csv:
        report_csv(st, num_designs) && num_designs++; break;
    case cmdline::fmt_pretty:
        report_pretty(st, num_designs) && num_designs++; break;
    }

    if (num_designs >= params.num_matches)
        return;
}

static void do_search(const ship& st_, ship& st, const cmdline& params, int& num_designs)
{
    if (params.use_big_engines)
        for (int F = params.fixed_engines.min; F <= params.engines.max; F++)
            for (int num_d30s = 0; num_d30s <= F; num_d30s++)
                for (int N = params.engines.min; N <= params.engines.max; N++)
                    for (int num_d30 = 0; num_d30 <= N; num_d30++)
                        for (int num_nk25 = 0; num_nk25 <= N - num_d30; num_nk25++)
                        {
                            int num_rd59 = N - num_d30 - num_nk25;
                            int num_rd51 = F - num_d30s;
                            do_search1(st_, st, params, { num_d30s, num_rd51, num_d30, num_nk25, num_rd59 }, num_designs);
                            if (num_designs >= params.num_matches)
                                return;
                        }
    else
        for (int num_d30s = params.fixed_engines.min; num_d30s <= params.fixed_engines.max; num_d30s++)
            for (int N = params.engines.min; N <= params.engines.max; N++)
                for (int num_d30 = 0; num_d30 <= N; num_d30++)
                {
                    int num_nk25 = N - num_d30;
                    do_search1(st_, st, params, { num_d30s, 0, num_d30, num_nk25, 0 }, num_designs);
                    if (num_designs >= params.num_matches)
                        return;
                }
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
        if (musl_optind == argc)
            cmdline::usage(argv[0]);
        for (int i = musl_optind; i < argc; i++)
            if (!add_gun(st, argv[i]))
            {
                INFO("Try '%s -G' to list supported guns.", params.argv[0]);
                terminate(EX_USAGE);
            }
        int nresults = 0;
        {
            ship copy;
            do_search(st, copy, params, nresults);
            if (params.use_big_tanks)
            {
                params.use_big_tanks = false;
                do_search(st, copy, params, nresults);
            }
        }

        if (nresults == 0)
        {
            WARN("no designs could be generated within the constraints.");
            return 1;
        }
        return 0;
    } catch (const exit_status& x) {
        return x.code;
    }
}

} // namespace hf::design
