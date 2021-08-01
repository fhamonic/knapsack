#ifndef UNBOUNDED_KNAPSTACK_INSTANCE_HPP
#define UNBOUNDED_KNAPSTACK_INSTANCE_HPP

#include <limits>
#include <vector>

#include "knapstack/instance.hpp"

namespace UnboundedKnapstack {
    template <typename Value, typename Cost>
    using Instance = Knapstack::Instance<Value, Cost>;
} //namespace UnboundedKnapstack

#endif //UNBOUNDED_KNAPSTACK_INSTANCE_HPP