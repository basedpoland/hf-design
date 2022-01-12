#include "cmdline.hpp"
#include "part.hpp"
#include "part-list.hpp"
#include "ship.hpp"
#include "log.hpp"

#include <tuple>

namespace hf::design {

void report_pretty(const ship& st, cmdline::fmt format)
{
    const auto& part_names = part::all_parts();
    const std::tuple<const char*, const part&, int> engine_parts[] = {
        { "d30s",   e_d30s,     2 },
        { "d30",    e_d30,      2 },
        { "nk25",   e_nk25,     2 },
    };

    printf("mass: %5.0f area:%4d cost:%6d twr:%4.1f time:%4.0f",
           (double)st.mass, st.area, st.cost, (double)st.twr(), (double)st.combat_time());
    if (format == cmdline::fmt_verbose)
    {
        printf("\n");
        for (const auto* part : part_names)
            if (int count = st.count(*part); count)
                printf("  %14s %3d %.1ft\n", part->name, count, (double)(part->mass * count));
    }
    else
    {
        printf(" |");
        for (const auto& [name, x, ndigits] : engine_parts)
            printf(" %s:%*d", name, -ndigits, st.count(x));
        printf(" pwr:%d,%d", st.count(pwr_1x2), st.count(pwr_2x2));
        printf(" tank:%2d,0(%-4.0ft)", st.count(tank_1x2), (double)(st.count(tank_1x2) * tank_1x2.mass)); // TODO big tank
        printf(" chassis:%d,%d", st.count(chassis_1), st.count(chassis_2));
        printf(" armor:%4.0f", (double)std::round(st.count(arm_1x1) * arm_1x1.mass));
        printf(".\n");
    }
}

} // namespace hf::design
