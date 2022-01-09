#include "cmdline.hpp"

#include "getopt.h"
#include "osdefs.hpp"
#include "part-list.hpp"
#include "log.hpp"

#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <utility>
#include <algorithm>

#define optarg musl_optarg
#define optind musl_optind
#define opterr musl_opterr
#define optopt musl_optopt

void cmdline::synopsis() const
{
    fprintf(stderr, "Try '%s -h' for more information.\n", argv[0]);
}

void cmdline::wrong_param() const
{
    fprintf(stderr, "%s: invalid argument '%s' for '%s'\n",
            argv[0],
            optarg ? optarg : "(null)",
            argv[optind-2]);
    synopsis();
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
        { "-F <tons>", "max fuel consumption"},
        { "-T <secs>", "min combat time" },
        { "-c <int>", "max cost"},
        { "-a <float>", "layers of armor assuming square ship"},
        { "-x <int>", "fire extinguisher amount" },
        { "-1", "exit immediately upon finding a match" },
        { "-n", "show up to that many matches"},
        { "-v", "increase verbosity level"},
        { "-q", "single-line output only"},
        { "-h, -?", "this screen" },
        { "-G", "help with gun names" },
        { "count:gun...", "use these guns on this ship" },
    };
    printf("usage: %s [opts] <count:gun-name>...\n", argv0);
    printf("this program generates HighFleet part lists.\n\n");
    for (const char* const* x : opts)
        printf("  %-22s        %s\n", x[0], x[1]);
    printf("\nexample: %s -f 6 -T 200 4:130mm\n", argv0);
    fflush(stdout);
    terminate(stdout == stderr ? EX_USAGE : 0);
}

void cmdline::gun_list(const cmdline& params)
{
    using pair = std::pair<const char*, const part*>;
    std::vector<pair> part_names{part::all_parts().cbegin(), part::all_parts().cend()};
    std::sort(part_names.begin(), part_names.end(), [](const pair& a, const pair& b) {
        return a.second->name < b.second->name;
    });
    printf("%s: supported gun parameters:\n", params.argv[0]);
    for (const auto& [_, part] : part_names)
        if (!strncmp(part->name, "g_", 2))
            printf("  %s\n", part->name+2);
    printf("\n");
    params.synopsis();
}

void cmdline::terminate(int status)
{
    throw exit_status(status);
}

cmdline cmdline::parse_options(int argc, char* const* argv)
{
    int c;
    cmdline p{ argc, argv};
    opterr = 1;

    while ((c = musl_getopt(argc, argv, "f:t:e:F:T:c:hGv1a:n:qx:")) != -1)
        switch (c)
        {
        default:
            err("unhandled command-line argument 0x%x'", (int)c);
            abort();
        case -1:
            if (optind == argc)
                usage(argv[0]);
            goto ok;
        case '?':
            //fprintf(stderr, "%s: unknown option '-%c'\n", argv[0], (char)optopt);
            goto error;
        case 'h':
            usage(argv[0]);
        case 'f': p.fixed_engine_count = p.get_int(0, 255); break;
        case 't': p.min_twr = p.get_float(1.1f, 50); break;
        case 'e': p.max_engines = p.get_int(1, 255); break;
        case 'F': p.max_fuel_usage = p.get_int(1, 10'000); break;
        case 'T': p.min_combat_time = p.get_int(10, 1000); break;
        case 'c': p.max_cost = p.get_int(1, 1'000'000); break;
        case '1': p.num_matches = 1; break;
        case 'n': p.num_matches = p.get_int(1); break;
        case 'a': p.armor_layers = p.get_float(0, 16); break;
        case 'x': p.num_extinguishers = p.get_int(0, 255); break;
        case 'q': p.verbosity = -1; break;
        case 'v': p.verbosity++; break;
        case 'G': gun_list(p); terminate(0);
        }
ok:
    return p;
error:
    p.synopsis();
    terminate(EX_USAGE);
}
