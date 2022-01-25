#include "part.hpp"
#include "part-list.hpp"
#include "ship.hpp"
#include "log.hpp"

#include <cmath>
#include <tuple>

namespace hf::design {

bool report_pretty(const ship& st, int)
{
    const std::tuple<const char*, const part&> engine_parts[] = {
        { "d30s",   e_d30s  },
        { "d30",    e_d30   },
        { "nk25",   e_nk25  },
        { "rd51",   e_rd51  },
        { "rd59",   e_rd59  },
    };

    printf("mass: %5.0f area:%4d fuel:%4.f cost:%6d twr:%4.1f htwr:%4.1f time:%4.0f |",
           (double)st.mass, st.area, (double)st.fuel_usage(), st.cost,
           (double)st.twr(), (double)st.horizontal_twr(), (double)st.combat_time());
    for (const auto& [name, x] : engine_parts)
        if (int cnt = st.count(x); cnt)
            printf(" %s:%d", name, cnt);
    printf(" pwr:%d,%d", st.count(pwr_1x2), st.count(pwr_2x2));
    printf(" tank:%2d,%d", st.count(tank_1x2), st.count(tank_4x4));
    printf(" legs:%d,%d", st.count(leg1), st.count(leg2));
    printf(" armor:%4.0f", (double)std::round(st.count(arm_1x1) * arm_1x1.mass));
    printf(".\n");

    return true;
}

} // namespace hf::design
