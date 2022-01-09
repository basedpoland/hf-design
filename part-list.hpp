#pragma once
#include "part.hpp"

#ifdef IN_PART_DECL
namespace {
// HACK: subtract 51 tons so that the data is easier to collect from the ship editor
static inline constexpr double operator""_t(long double x) { return (double)x - 51; }
static inline constexpr double operator""_t(unsigned long long x) { return (double)x - 51; }
}
#endif

#ifdef IN_PART_DECL
#   define PART(name, ...) extern const part name; const part name { (#name), __VA_ARGS__ }
#else
#   define PART(name, ...) extern const part name;
#endif

//   name       mass    power   size    cost    thrust   fuel    ammo
PART(g_37mm,    102_t, -0.7,    sz_2x2, 3000,   0,      0,     -1   );
PART(g_57mm,    102_t, -0.7,    sz_2x2, 2000,   0,      0,     -1   );
PART(g_100mm,   102_t, -1,      sz_2x2, 2000,   0,      0,     -1   );
PART(g_130mm,   102_t, -1,      sz_2x2, 4000,   0,      0,     -1   );
PART(g_180mm,   132_t, -1.8,    sz_2x2, 4000,   0,      0,     -2   );
PART(g_180mmx2, 132_t, -2.4,    sz_2x2, 6000,   0,      0,     -4   );
PART(ammo_1x2,  107.2,   -0.8,  sz_1x2, 500,    0,      0,      1   );
PART(ammo_2x2,  197.2,   -1.6,  sz_2x2, 1000,   0,      0,      2   );
PART(arm_1x1,   64.3,   0,      sz_1x1, 200,    0,      0,      0   );
PART(bridge,    25.8,   0,      sz_2x2, 0,      0,      0,      0   );
PART(e_d30,     74_t,  -1.2,    sz_cor, 1000,   12.5,   -0.15       );
PART(e_nk25,    81_t,  -1.3,    sz_cor, 1500,   18,     -0.32       );
PART(e_d30s,    70_t,  -0.2,    sz_2x2, 1000,   21,     -0.2        );
PART(tank_1x2,  37.3,   0,      sz_1x2, 10,     0,      40          );
PART(tank_4x4,  430.6,  0,  sz_bigfuel, 80,     0,      450         );
PART(h_2x2,     25.8,   0,      sz_nan, 20                          );
PART(h_cor,     25.8,   0,      sz_nan, 20                          );
PART(h_1x1,     5.6,    0,      sz_nan, 5                           );
PART(h_1x2,     11.2,   0,      sz_nan, 10                          );
PART(h_05,      5.6,    0,      sz_nan, 5                           );
PART(h_null,    0,      0,      sz_nan, 0                           );
PART(fire,      31.3,   0,      sz_1x2, 300                         );
PART(chassis_1, 2.8,    -0.05,  sz_zro, 50                          );
PART(chassis_2, 17.9,   -0.1,   sz_cor, 100                         );
PART(pwr_1x2,   43.3,   2.8,    sz_1x2, 150                         );
PART(pwr_2x2,   93.2,   6.1,    sz_2x2, 200                         );
PART(null_part, 0,      0,      sz_nan, 0                           );

#undef PART

