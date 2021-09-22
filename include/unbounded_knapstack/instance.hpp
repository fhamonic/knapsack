#ifndef UNBOUNDED_KNAPSTACK_INSTANCE_HPP
#define UNBOUNDED_KNAPSTACK_INSTANCE_HPP

#include <limits>
#include <vector>

#include "knapstack/instance.hpp"

namespace fhamonic {
namespace unbounded_knapstack {

template <typename Value, typename Cost>
using Instance = knapstack::Instance<Value, Cost>;

}  // namespace unbounded_knapstack
}  // namespace fhamonic

#endif  // UNBOUNDED_KNAPSTACK_INSTANCE_HPP