#undef NDEBUG
#include "cmdline.hpp"

#include "getopt.h"
#include "defs.hpp"
#include "part.hpp"
#include "log.hpp"

#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <utility>
#include <tuple>

#define optarg musl_optarg
#define optind musl_optind
#define opterr musl_opterr
#define optopt musl_optopt

namespace hf::design {

void cmdline::synopsis(const char* argv0)
{
    printf("usage: %s [opts] <count:gun-name>...\n", argv0);
}

void cmdline::seek_help() const
{
    fprintf(stderr, "Try '%s -h' for more information.\n", argv[0]);
}

void cmdline::wrong_param(const char* explain) const
{
    fprintf(stderr, "%s: invalid argument '%s' for '%s'%s\n",
            argv[0],
            optarg ? optarg : "(null)",
            argv[optind-2],
            explain);
    seek_help();
    terminate(EX_USAGE);
}

int cmdline::get_int(int min, int max) const
{
    if (!optarg)
        wrong_param();
    char* end;
    errno = 0;
    int x = (int)std::strtol(optarg, &end, 10);
    if (end == optarg || *end || errno == ERANGE)
        wrong_param();
    if (x < min)
        wrong_param(" (too small)");
    if (x > max)
        wrong_param(" (too large)");
    return x;
}

float cmdline::get_float(float min, float max) const
{
    if (!optarg)
        wrong_param();
    char* end;
    errno = 0;
    float x = std::strtof(optarg, &end);
    if (end == optarg || *end || errno == ERANGE)
        wrong_param();
    if (x < min)
        wrong_param(" (too small)");
    if (x > max)
        wrong_param(" (too large)");
    return x;
}

void cmdline::usage(const char* argv0)
{
    constexpr const char* opts[][2] = {
        { "-f <int>",                   "fixed thruster count"                  },
        { "-T <float>",                 "minimum twr"                           },
        { "-H <float>",                 "minimum horizontal twr"                },
        { "-e <int>",                   "max maneuvering engines"               },
        { "-u <tons>",                  "max fuel consumption"                  },
        { "-t <secs>",                  "min combat time"                       },
        { "-c <int>",                   "max cost"                              },
        { "-a <float>",                 "layers of armor assuming square ship"  },
        { "-x <int>",                   "fire extinguisher amount"              },
        { "-b",                         "enable large tanks"                    },
        { "-B",                         "enable large engines"                  },
        { "-m <float>",                 "add extra mass"                        },
        { "-p <float>",                 "add extra power requirement"           },
        { "-P <float>",                 "provide less than 100% power"          },
        { "-C [<nlegs>:]n1,n2,n3,n4",   "how many chassis parts to use"         },
        {},
        { "-F <pretty|csv>",            "output format"                         },
        { "-n",                         "output limit"                          },
        { "-h, -?",                     "this screen"                           },
        { "-G", "help with gun names"                                           },
        {},
        { "count:gun...", "use these guns on the ship"                          },
    };
    synopsis(argv0);
    printf("this program generates HighFleet part lists.\n\n");
    for (const char* const* x : opts)
    {
        if (!x[0])
            putchar('\n');
        else
            printf("  %-29s %s\n", x[0], x[1]);
    }
    printf("\nexample: %s -F csv -b -x 2 -e 4:16 -f 4:8 -T 200 -P 0.99 -a 1.3 4:130mm\n", argv0);
    fflush(stdout);
    terminate(stdout == stderr ? EX_USAGE : 0);
}

void cmdline::gun_list() const
{
    const auto& part_names = part::all_parts();
    printf("%s: supported gun parameters:\n", argv[0]);
    synopsis(argv[0]);
    for (const auto* part : part_names)
        if (!strncmp(part->name, "g_", 2))
            printf("  %s\n", part->name+2);
    printf("\n");
    seek_help();
}

cmdline cmdline::parse_options(int argc, const char* const* argv)
{
    int c;
    cmdline p{argc, argv};
    opterr = 1;

    while ((c = musl_getopt(argc, argv, "f:T:H:e:u:t:c:hGa:n:x:F:bm:p:BP:C:")) != -1)
        switch (c)
        {
        default:
            ABORT("unhandled command-line argument -- '%c'(0x%x)'", (char)c, c);
            break;
        case -1:
            if (optind == argc)
                usage(argv[0]);
            goto ok;
        case ':':
        case '?':
            //ERR("unknown option '-%c'\n", (char)optopt);
            goto error;
        case 'h':
            usage(argv[0]);
        case 'f': p.fixed_engines.parse(c, optarg); break;
        case 'T': p.twr.parse(c, optarg); break;
        case 'H': p.horizontal_twr.parse(c, optarg); break;
        case 'e': p.engines.parse(c, optarg); break;
        case 'u': p.fuel_usage.parse(c, optarg); break;
        case 't': p.combat_time = p.get_int(1, 1 << 16); break;
        case 'c': p.cost.parse(c, optarg); break;
        case 'G': p.gun_list(); terminate(0);
        case 'a': p.armor_layers = p.get_float(0, 16); break;
        case 'n': p.num_matches = p.get_int(0); if (!p.num_matches) p.num_matches = INT_MAX; break;
        case 'x': p.num_extinguishers = p.get_int(0, 255); break;
        case 'F': p.format = p.parse_format(optarg); break;
        case 'b': p.use_big_tanks = true; break;
        case 'm': p.extra_mass += p.get_float(-1e12f, 1e12f); break;
        case 'p': p.extra_power += p.get_float(0, 1e3f); break;
        case 'B': p.use_big_engines = true; p.use_big_tanks = true; break;
        case 'P': p.power = p.get_float(0, 1); break;
        case 'C': p.chassis = p.parse_chassis_layout(optarg); break;
        }
ok:
    return p;
error:
    p.seek_help();
    terminate(EX_USAGE);
}

cmdline::fmt cmdline::parse_format(const char* str) const
{
    const std::pair<const char*, fmt> formats[] = {
        { "pretty",     fmt_pretty  },
        { "csv",        fmt_csv     },
    };
    for (const auto& [name, fmt] : formats)
        if (!strcmp(str, name))
            return fmt;

    ERR("invalid output format -- '%s'", optarg);
    seek_help();
    terminate(EX_USAGE);
}

#define BAD_CHASSIS "invalid chassis spec -- "

cmdline::chassis_layout cmdline::parse_chassis_layout(const char* str)
{
    char buf[64];
    chassis_layout ret = { 0, { 0, 0, 0, 0 } };
    auto& [nlegs, array] = ret;
    char* pos;

    auto parse = [this](const char* x) {
        char* endptr;
        errno = 0;
        int ret = string_to_type<int>(x, &endptr);
        if (endptr == x || *endptr || errno)
            ERR(BAD_CHASSIS "can't parse integer at '%s'", x);
        else if (ret < 0)
            ERR(BAD_CHASSIS "must be non-negative integer");
        else
            return ret;

        seek_help();
        terminate(EX_USAGE);
    };

    if (strlen(str) >= sizeof(buf))
    {
        ERR(BAD_CHASSIS "invalid length");
        goto error;
    }
    strcpy(buf, str);
    buf[sizeof(buf)-1] = '\0';
    pos = strchr(buf, ':');

    if (pos)
    {
        *pos++ = '\0';
        nlegs = parse(buf);

        if (!*pos)
        {
            ERR(BAD_CHASSIS "missing part list");
            goto error;
        }
    }
    else
        pos = buf;

    for (unsigned i = 0; pos && i <= std::size(array); i++)
    {
        char* next = strchr(pos, ',');
        if (i == std::size(array))
        {
            ERR(BAD_CHASSIS "too many elements in part list");
            goto error;
        }
        if (next)
            *next++ = '\0';
        array[i] = parse(pos);
        pos = next;
    }

    return ret;
error:
    seek_help();
    terminate(EX_USAGE);
}

} // namespace hf::design
