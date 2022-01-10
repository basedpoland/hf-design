#include "state.hpp"
#include "part-list.hpp"

namespace hf::design {

int state::count(const part& x) const
{
    auto it = parts.find(&x);
    if (it != parts.end())
        return it->second;
    else
        return 0;
}

state::state()
{
    add_part_(*this, bridge);
}

} // namespace hf::design
