#pragma once
#include <limits>
#include <tuple>

namespace hf::design {

enum class range_behavior : unsigned char { min, max, same };

template<typename t>
struct range final
{
    t min = std::numeric_limits<t>::min();
    t max = std::numeric_limits<t>::max();
    range_behavior r;

    constexpr bool check(t x) const { return x >= min && x <= max; }
    explicit range(range_behavior r = range_behavior::min);
    range(t min, t max, range_behavior r = range_behavior::min);
    range<t>& operator=(const std::tuple<t, t>& x);
    void parse(int c);
};

struct cmdline final
{
    using frange = range<float>;
    using irange = range<int>;

    enum fmt : char {
        fmt_pretty = 1,
        fmt_csv,
        fmt_verbose,
        fmt_default = fmt_pretty
    };

    static constexpr auto float_min = std::numeric_limits<float>::min();
    static constexpr auto float_max = std::numeric_limits<float>::max();
    static constexpr auto int_min = std::numeric_limits<int>::min();
    static constexpr auto int_max = std::numeric_limits<int>::max();

    frange twr{1.1f, float_max};
    irange engines{1, 32, range_behavior::max};
    frange fuel_usage{0, float_max, range_behavior::max};
    int combat_time = 200;
    irange cost{0, int_max, range_behavior::max};
    irange fixed_engines{2, 6, range_behavior::same};

    float armor_layers = 0;
    float extra_mass = 0;
    float extra_power = 0;
    const char* const* argv = nullptr;
    int argc = 0;
    int num_matches = std::numeric_limits<int>::max();
    int num_extinguishers = 2;
    fmt format = fmt_default;
    bool use_big_tanks = false;
    bool use_big_engines = false;

    static cmdline parse_options(int argc, const char* const* argv);
    [[noreturn]] void wrong_param() const;

    int get_int(int min = 0, int max = 1 << 16) const;
    float get_float(float min = 0, float max = 1 << 16) const;
    [[nodiscard]] fmt parse_format(const char* str) const;
    void seek_help() const;
    void gun_list() const;
    static void synopsis(const char* argv0);
    [[noreturn]] static void usage(const char* argv0);
    [[noreturn]] static void terminate(int status);

private:
    cmdline() = default;
    cmdline(int argc, const char* const* argv) : argv(argv), argc(argc) {}
};

extern template struct range<float>;
extern template struct range<int>;

struct exit_status final
{
    int code;
    explicit exit_status(int exit_code) : code(exit_code) {}
};

} // namespace hf::design
