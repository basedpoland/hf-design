#include "part-list.hpp"
#include "ship.hpp"
#include <cstdio>
#include <variant>
#include <numeric>

namespace hf::design {

struct line final
{
    explicit line(FILE* stream) : stream{stream} {}
    void sep();
    template<typename T> line& operator<<(T);

private:
    template<typename T> void write(T x);
    FILE* stream;
    bool first_column = true;
};

template<typename T> line& line::operator<<(T x) { sep(); write(x); return *this; }

void line::sep()
{
    if (!first_column)
        putc(',', stream);
    first_column = false;
}

using float_format = std::tuple<float, int>;

template<> void line::write(float x) { fprintf(stream, "%.1f", (double)x); }
template<> void line::write(float_format x) { auto [f, p] = x; fprintf(stream, "%.*f", p, (double)f); }
template<> void line::write(int x) { fprintf(stream, "%d", x); }
template<> void line::write(char x) { putc(x, stream); }
template<> void line::write(const char* x) { fprintf(stream, "%s", x); }

bool report_csv(const ship& st, int k)
{
    using variant = std::variant<int, float, float_format>;
    auto mass_of = [&](const part& x) { return st.count(x) * x.mass; };
    auto count_of = [&](const part& x) { return st.count(x); };

    const std::tuple<const char*, variant> values[] = {
        { "Cost",           st.cost                                 },
        { "Mass",           st.mass                                 },
        { "TWR",            float_format{st.twr(), 2}               },
        { "hTWR",           float_format{st.horizontal_twr(), 2}    },
        { "Combat time",    st.combat_time(),                       },
        { "Speed",          st.speed(),                             },
        { "Range",          st.range()                              },
        { "Fuel usage",     st.fuel_usage()                         },
        { "Armor",          mass_of(arm_1x1)                        },
        { "Fuel",           (int)st.fuel                            },
        //{ "Tanks (tons)",   mass_of(tank_1x2) + mass_of(tank_4x4)   },
        { "D-30s",          count_of(e_d30s)                        },
        { "D-30",           count_of(e_d30)                         },
        { "NK-25",          count_of(e_nk25)                        },
        { "RD-51",          count_of(e_rd51)                        },
        { "RD-59",          count_of(e_rd59)                        },
        { "Tank L",         count_of(tank_4x4)                      },
        { "Tank S",         count_of(tank_1x2)                      },
        { "Power S",        count_of(pwr_1x2)                       },
        { "Power L",        count_of(pwr_2x2)                       },
        { "Leg(1)",         count_of(leg1),                         },
        { "Leg(2)",         count_of(leg2),                         },
        { "Leg(3)",         count_of(leg3),                         },
        { "Leg(4)",         count_of(leg4),                         },
    };

    if (k == 0)
    {
        line s{stdout};
        for (const auto& [name, _] : values)
            s << name;
        putchar('\n');
    }

    line s{stdout};
    auto print = [&] (const auto& x) { s << x; };

    for (const auto& [_, x] : values)
        std::visit(print, x);
    putchar('\n');

    return true;
}

} // namespace hf::design
