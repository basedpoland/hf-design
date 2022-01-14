#pragma once
#include "part.hpp"

#ifdef IN_PART_DECL
#   define PART(name, ...) extern const part name; const part name { (#name), __VA_ARGS__ }
#else
#   define PART(name, ...) extern const part name
#endif

namespace hf::design {

//   name       mass        power   size    cost    thrust  fuel    ammo
PART(g_37mm,    51.1427,    -0.7,   sz_2x2, 3000,   0,      0,      -1  );
PART(g_57mm,    51.1427,    -0.7,   sz_2x2, 2000,   0,      0,      -1  );
PART(g_100mm,   51.1427,    -1,     sz_2x2, 2000,   0,      0,      -1  );
PART(g_130mm,   51.1427,    -1,     sz_2x2, 4000,   0,      0,      -1  );
PART(g_180mm,   81.2129,    -1.8,   sz_2x2, 4000,   0,      0,      -2  );
PART(g_180mmx2, 81.2129,    -2.4,   sz_2x2, 6000,   0,      0,      -4  );
PART(ammo_1x2,  107.239,    -0.8,   sz_1x2, 500,    0,      0,      1   );
PART(ammo_2x2,  197.294,    -1.6,   sz_2x2, 1000,   0,      0,      2   );
PART(arm_1x1,   64.3224,    0,      sz_1x1, 200,    0,      0,      0   );
PART(bridge,    25.8142,    0,      sz_2x2, 0,      0,      0,      0   );
PART(e_d30,     22.5695,    -1.2,   sz_cor, 1000,   12.5,   -0.15       );
PART(e_nk25,    30.0763,    -1.3,   sz_cor, 1500,   18,     -0.3125     );
PART(e_d30s,    18.5879,    -0.2,   sz_2x2, 1000,   21,     -0.2        );
PART(tank_1x2,  37.3006,    0,      sz_1x2, 10,     0,      40          );
PART(tank_4x4,  430.659,    0,  sz_bigfuel, 80,     0,      450         );
PART(h_null,    0,          0,      sz_nan, 0                           );
PART(h_05,      5.687,      0,      sz_nan, 5                           );
PART(h_1x1,     5.687,      0,      sz_nan, 5                           );
PART(h_1x2,     11.2612,    0,      sz_nan, 10                          );
PART(h_2x2,     25.8142,    0,      sz_nan, 20                          );
PART(h_cor,     25.8142,    0,      sz_nan, 20                          );
PART(h_4x4,     574.212,    0,      sz_nan, 80,                         );
PART(fire,      31.3276,    0,      sz_1x2, 300                         );
PART(chassis_1, 2.83583,    -0.05,  sz_nan, 50                          );
PART(chassis_2, 17.9923,    -0.1,   sz_cor, 100                         );
PART(pwr_1x2,   43.3147,    2.8,    sz_1x2, 150                         );
PART(pwr_2x2,   93.252,     6.1,    sz_2x2, 200                         );
PART(null_part, 0,          0,      sz_nan, 0                           );
PART(rh_1x2,    45.456,     0,      sz_nan, 100                         );
PART(rh_1x1,    22.7505,    0,      sz_nan, 50                          );
PART(rh_2x2,    103.257,    0,      sz_nan, 200                         );


#undef PART

} // namespace hf::design
