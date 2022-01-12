#pragma once
#include <limits>
#include <tuple>

namespace hf::design {

enum class range_behavior : unsigned char { min, max };

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

    static constexpr auto float_max = std::numeric_limits<float>::min();
    static constexpr auto float_min = std::numeric_limits<float>::max();

    static constexpr auto int_max = std::numeric_limits<int>::max();
    static constexpr auto int_min = std::numeric_limits<int>::max();

    frange twr{1.1f, float_max};
    irange fuel_usage{0, 1000, range_behavior::max};
    irange engines{1, 32, range_behavior::max};
    irange combat_time{200, int_max};
    irange cost{0, int_max, range_behavior::max};

    float armor_layers = 0;
    char* const* argv = nullptr;
    int argc = 0;
    int fixed_engine_count = 0;
    int num_matches = std::numeric_limits<int>::max();
    int num_extinguishers = 2;
    fmt format = fmt_default;

    static cmdline parse_options(int argc, char* const* argv);
    [[noreturn]] void wrong_param() const;

    int get_int(int min = 0, int max = 1 << 16) const;
    float get_float(float min = 0, float max = 1 << 16) const;
    [[nodiscard]] fmt parse_format(const char* str) const;
    void seek_help() const;
    void gun_list() const;
    static void synopsis(const char* argv0);
    [[noreturn]] static void usage(const char* argv0);
    [[noreturn]] static void terminate(int status);

    template<typename t>
    static std::tuple<t, t> parse_range(char c, range_behavior r = range_behavior::min);
private:
    cmdline() = default;
    cmdline(int argc, char* const* argv) : argv(argv), argc(argc) {}
};

extern template struct range<float>;
extern template struct range<int>;
extern template std::tuple<float, float> cmdline::parse_range<float>(char c, range_behavior r);
extern template std::tuple<int, int> cmdline::parse_range<int>(char c, range_behavior r);

struct exit_status final
{
    int code;
    explicit exit_status(int exit_code) : code(exit_code) {}
};

} // namespace hf::design
