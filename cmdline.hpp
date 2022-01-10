#pragma once
#include <limits>

struct cmdline final
{
    static constexpr inline auto int_max = std::numeric_limits<int>::max();

    float min_twr = 2, armor_layers = 0;
    char* const* argv = nullptr;
    int argc = 0;
    int fixed_engine_count = 0, max_engines = 32, max_fuel_usage = int_max;
    int min_combat_time = 200, max_cost = int_max;
    int verbosity = 0;
    int num_matches = std::numeric_limits<int>::max();
    int num_extinguishers = 2;

    static cmdline parse_options(int argc, char* const* argv);
    [[noreturn]] void wrong_param() const;

    int get_int(int min = 0, int max = 1 << 16) const;
    float get_float(float min = 0, float max = 1 << 16) const;
    void seek_help() const;
    void gun_list() const;
    static void synopsis(const char* argv0);
    [[noreturn]] static void usage(const char* argv0);
    [[noreturn]] static void terminate(int status);
private:
    cmdline() = default;
    cmdline(int argc, char* const* argv) : argv(argv), argc(argc) {}
};

struct exit_status final
{
    int code;
    explicit exit_status(int exit_code) : code(exit_code) {}
};


