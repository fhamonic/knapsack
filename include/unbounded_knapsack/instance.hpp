#ifndef UNBOUNDED_FHAMONIC_KNAPSACK_INSTANCE_HPP
#define UNBOUNDED_FHAMONIC_KNAPSACK_INSTANCE_HPP

#include <limits>
#include <vector>

#include "knapsack/instance.hpp"

namespace fhamonic {
namespace unbounded_knapsack {

template <typename Value, typename Cost>
using Instance = knapsack::Instance<Value, Cost>;

}  // namespace unbounded_knapsack
}  // namespace fhamonic

#endif  // UNBOUNDED_FHAMONIC_KNAPSACK_INSTANCE_HPP