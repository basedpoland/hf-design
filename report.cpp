#include "cmdline.hpp"
#include "part.hpp"
#include "part-list.hpp"
#include "state.hpp"
#include "log.hpp"

namespace hf::design {

template<typename F>
static void csv_foreach_part(F&& fn)
{
    const std::tuple<const char*, const part&> csv_parts[] = {
        { "D-30s",          e_d30s      },
        { "D-30",           e_d30       },
        { "NK-25",          e_nk25      },
        { "Tank (small}",   tank_1x2    },
        { "Chassis (1)",    chassis_1,  },
        { "Chassis (2)",    chassis_2,  },
        { "Armor",          arm_1x1     },
    };

    bool first = true;
    for (const auto& [name, part] : csv_parts)
    {
        if (!first)
            putchar(',');
        fn(name, part);
        first = false;
    }
}

static bool filter_ship(const state& st, const cmdline& params)
{
    bool ret = true;
    ret &= st.twr() >= params.min_twr;
    ret &= st.cost <= params.max_cost;

    return ret;
}

bool report(const state& st, const cmdline& params, int k)
{
    if (!filter_ship(st, params))
        return false;

    const auto& part_names = part::all_parts();
    switch (params.format)
    {
    default: BUG("unhandled format type 0x%02ux", (unsigned)params.format);
    case cmdline::fmt_csv:
    {
        if (k == 0)
        {
            csv_foreach_part([] (const char* name, const part&) {
                fputs(name, stdout);
            });
            putchar('\n');
        }
        csv_foreach_part([&] (const char*, const part& x) {
            printf("%d", st.count(x));
        });
        putchar('\n');
        break;
    }
    case cmdline::fmt_verbose:
    case cmdline::fmt_pretty:
        const std::tuple<const char*, const part&, int> engine_parts[] = {
            { "d30s",   e_d30s,     2 },
            { "d30",    e_d30,      2 },
            { "nk25",   e_nk25,     2 },
            { "armor",  arm_1x1,    3 },
        };

        printf("mass: %5.0f area:%4d cost:%6d twr:%4.1f time:%4.0f",
               (double)st.mass, st.area, st.cost, (double)st.twr(), (double)st.combat_time());
        if (params.format == cmdline::fmt_verbose)
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
            printf(" tank:%2d,0", st.count(tank_1x2)); // TODO big tank
            printf(" chassis:%d,%d", st.count(chassis_1), st.count(chassis_2));
            printf(".\n");
        }
        break;
    }
    return true;
}

} // namespace hf::design
