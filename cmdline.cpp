#undef NDEBUG
#include "cmdline.hpp"

#include "getopt.h"
#include "osdefs.hpp"
#include "part.hpp"
#include "log.hpp"

#include <cassert>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <utility>
#include <tuple>

#ifdef _WIN32
#   include <malloc.h>
#   ifdef _MSC_VER
#       define alloca _alloca
#endif
#else
#   include <alloca.h>
#endif

#define optarg musl_optarg
#define optind musl_optind
#define opterr musl_opterr
#define optopt musl_optopt

namespace hf::design {

template<typename t> struct conv_for_type;

template<typename t>
static t string_to_type(const char* str, char** endptr)
{
    errno = 0;
    return conv_for_type<t>::conv(str, endptr);
}

template<> struct conv_for_type<float>
{
    static float conv(const char* str, char** endptr) { return std::strtof(str, endptr); }
};
template<> struct conv_for_type<int>
{
    static int conv(const char* str, char** endptr) { return std::strtol(str, endptr, 10); }
};

template<typename t>
static std::tuple<t, t, bool> parse_range_(const char* str, range_behavior r)
{
    constexpr auto min = std::numeric_limits<t>::min();
    constexpr auto max = std::numeric_limits<t>::max();
    const char* sep = strchr(str, ':');
    char* endptr;
    errno = 0;
    if (!sep) // lone number
    {
        auto x = string_to_type<t>(str, &endptr);
        if (*endptr || errno)
            return {};
        switch (r)
        {
        default:
        case range_behavior::same: return { x, x, true  };
        case range_behavior::max:  return { min, x, true };
        case range_behavior::min:  return { x, max, true };
        }

    }
    else if (sep - str == 0) // inf -> x
    {
        auto x = string_to_type<t>(str+1, &endptr);
        if (*endptr || errno)
            return {};
        return { min, x, true };
    }
    else if (sep[1] == '\0') // x -> inf
    {
        auto x = string_to_type<t>(str, &endptr);
        if (endptr != sep || errno)
            return {};
        return { x, max, true };
    }
    else
    {
        auto start = string_to_type<t>(str, &endptr);
        if (endptr != sep || errno)
            return {};
        auto end = string_to_type<t>(sep+1, &endptr);
        if (*endptr || errno || end < start)
            return {};
        return { start, end, true };
    }
}

template<typename t>
void range<t>::parse(int c)
{
    assert(optarg);

    auto [min_, max_, ok] = parse_range_<t>(optarg, r);
    if (!ok)
    {
        ERR("invalid range given to -%c: '%s'", (char)c, optarg);
        cmdline::terminate(EX_USAGE);
    }
    min = min_;
    max = max_;
}

void cmdline::synopsis(const char* argv0)
{
    printf("usage: %s [opts] <count:gun-name>...\n", argv0);
}

void cmdline::seek_help() const
{
    fprintf(stderr, "Try '%s -h' for more information.\n", argv[0]);
}

void cmdline::wrong_param() const
{
    fprintf(stderr, "%s: invalid argument '%s' for '%s'\n",
            argv[0],
            optarg ? optarg : "(null)",
            argv[optind-2]);
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
    if (x < min || x > max)
        wrong_param();
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
    if (x < min || x > max)
        wrong_param();
    return x;
}

void cmdline::usage(const char* argv0)
{
    constexpr const char* opts[][2] = {
        { "-f <int>", "fixed thruster count" },
        { "-t <float>", "minimum twr" },
        { "-e <int>", "max maneuvering engines"},
        { "-u <tons>", "max fuel consumption"},
        { "-T <secs>", "min combat time" },
        { "-c <int>", "max cost"},
        { "-a <float>", "layers of armor assuming square ship"},
        { "-x <int>", "fire extinguisher amount" },
        {},
        { "-F <pretty|line|verbose>", "output format"},
        { "-1", "exit immediately upon finding a match" },
        { "-n", "output limit"},
        { "-h, -?", "this screen" },
        { "-G", "help with gun names" },
        {},
        { "count:gun...", "use these guns on this ship" },
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
    printf("\nexample: %s -f 6 -T 200 4:130mm\n", argv0);
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

void cmdline::terminate(int status)
{
    throw exit_status(status);
}

cmdline cmdline::parse_options(int argc, const char* const* argv)
{
    int c;
    cmdline p{argc, argv};
    opterr = 1;

    while ((c = musl_getopt(argc, argv, "f:t:e:u:T:c:hG1a:n:x:F:bm:p:")) != -1)
        switch (c)
        {
        default:
            BUG("unhandled command-line argument -- '%c'(0x%x)'", (char)c, c);
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
        case 'f': p.fixed_engines.parse(c); break;
        case 't': p.twr.parse(c); break;
        case 'e': p.engines.parse(c); break;
        case 'u': p.fuel_usage.parse(c); break;
        case 'T': p.combat_time = p.get_int(1, 1 << 16); break;
        case 'c': p.cost.parse(c); break;
        case '1': p.num_matches = 1; break;
        case 'n': p.num_matches = p.get_int(0); if (!p.num_matches) p.num_matches = INT_MAX; break;
        case 'a': p.armor_layers = p.get_float(0, 16); break;
        case 'x': p.num_extinguishers = p.get_int(0, 255); break;
        case 'G': p.gun_list(); terminate(0);
        case 'F': p.format = p.parse_format(optarg); break;
        case 'b': p.use_big_tanks = true; break;
        case 'm': p.extra_mass += p.get_float(-1e12f, 1e12f); break;
        case 'p': p.extra_power += p.get_float(0, 1e3f); break;
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
        { "verbose",    fmt_verbose },
    };
    for (const auto& [name, fmt] : formats)
        if (!strcmp(str, name))
            return fmt;

    ERR("invalid output format -- '%s'", optarg);
    seek_help();
    terminate(EX_USAGE);
}

template<typename t>
range<t>::range(range_behavior r) : r(r) {}

template<typename t>
range<t>::range(t min, t max, range_behavior r) : min(min), max(max), r(r) {}

template<typename t>
range<t>& range<t>::operator=(const std::tuple<t, t>& x)
{
    std::tie(min, max) = x;
    return *this;
}

template struct range<float>;
template struct range<int>;

} // namespace hf::design
