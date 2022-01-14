#include "cmdline.hpp"
#include "part.hpp"
#include "part-list.hpp"
#include "ship.hpp"
#include "log.hpp"

#include <tuple>

namespace hf::design {

void report_pretty(const ship& st, cmdline::fmt format, int)
{
    const auto& part_names = part::all_parts();
    const std::tuple<const char*, const part&> engine_parts[] = {
        { "d30s",   e_d30s  },
        { "d30",    e_d30   },
        { "nk25",   e_nk25  },
        { "rd51",   e_rd51  },
        { "rd59",   e_rd59  },
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
        for (const auto& [name, x] : engine_parts)
            if (int cnt = st.count(x); cnt)
                printf(" %s:%-2d", name, cnt);
        printf(" pwr:%d,%d", st.count(pwr_1x2), st.count(pwr_2x2));
        printf(" tank:%2d,0(%-4.0ft)", st.count(tank_1x2), (double)(st.count(tank_1x2) * tank_1x2.mass)); // TODO big tank
        printf(" legs:%d,%d", st.count(leg1), st.count(leg2));
        printf(" armor:%4.0f", (double)std::round(st.count(arm_1x1) * arm_1x1.mass));
        printf(".\n");
    }
}

} // namespace hf::design
