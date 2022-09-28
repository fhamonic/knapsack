#ifndef FHAMONIC_KNAPSACK_DYNAMIC_PROGRAMMING_HPP
#define FHAMONIC_KNAPSACK_DYNAMIC_PROGRAMMING_HPP

#include <algorithm>
#include <numeric>
#include <type_traits>

#include <range/v3/view/reverse.hpp>

#include "knapsack/instance.hpp"
#include "knapsack/solution.hpp"

namespace fhamonic {
namespace knapsack {

template <
    typename Value, typename Cost,
    class = typename std::enable_if<std::is_integral<Cost>::value, bool>::type>
class DynamicProgramming {
public:
    using TInstance = Instance<Value, Cost>;
    using TItem = typename TInstance::Item;
    using TSolution = Solution<Value, Cost>;

public:
    DynamicProgramming() {}

    TSolution solve(const TInstance & instance) {
        const std::size_t nb_items = instance.itemCount();
        const Cost budget = instance.getBudget();

        auto tab = std::make_unique<Value[]>(
            (nb_items + 1) * static_cast<std::size_t>(budget + 1));

        Value * previous_tab = tab.get();
        for(Cost w = 0; w < budget; ++w) {
            previous_tab[w] = 0;
        }

        for(const auto & item : instance.getItems()) {
            Value * const current_tab = previous_tab + budget + 1;
            Cost w = std::min(budget, item.cost);
            std::copy(previous_tab, previous_tab + w, current_tab);
            for(; w <= budget; ++w) {
                current_tab[w] = std::max(
                    previous_tab[w], previous_tab[w - item.cost] + item.value);
            }
            previous_tab = current_tab;
        }

        TSolution solution(instance);
        const Value * step = previous_tab + budget;
        for(std::size_t i = (nb_items - 1); i > 0; --i) {
            const bool taken = (*step > *(step - budget - 1));
            solution.set(i, taken);
            step -= budget + 1 + taken * instance[i].cost;
        }
        const bool taken = (*step > *(step - budget - 1));
        solution.set(0, taken);

        return solution;
    }
};

}  // namespace knapsack
}  // namespace fhamonic

#endif  // FHAMONIC_KNAPSACK_DYNAMIC_PROGRAMMING_HPP