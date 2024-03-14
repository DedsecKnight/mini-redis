#pragma once

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

namespace lib::data_types {

template <typename elem_t>
using ordered_set =
    __gnu_pbds::tree<elem_t, __gnu_pbds::null_type, std::less<elem_t>,
                     __gnu_pbds::rb_tree_tag,
                     __gnu_pbds::tree_order_statistics_node_update>;

}  // namespace lib::data_types