#ifndef KNAPSTACK_DYNAMIC_PROGRAMMING_HPP
#define KNAPSTACK_DYNAMIC_PROGRAMMING_HPP

#include <numeric>
#include <type_traits>

#include <range/v3/view/reverse.hpp>

#include "knapstack/instance.hpp"
#include "knapstack/solution.hpp"

namespace Knapstack {
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
        const int nb_items = instance.itemCount();
        const Cost budget = instance.getBudget();

        auto tab = std::make_unique<Value[]>((nb_items + 1) * (budget + 1));

        Value * previous_tab = tab.get();
        for(Cost w = 0; w < budget; ++w) {
            previous_tab[w] = 0;
        }

        for(const auto & item : instance.getItems()) {
            Value * const current_tab = previous_tab + budget + 1;
            Cost w = 0;
            for(; w < std::min(budget, item.cost); ++w)
                current_tab[w] = previous_tab[w];
            for(; w <= budget; ++w) {
                current_tab[w] = std::max(
                    previous_tab[w], previous_tab[w - item.cost] + item.value);
            }
            previous_tab = current_tab;
        }

        TSolution solution(instance);
        const Value * step = previous_tab + budget;
        for(int i = nb_items - 1; i >= 0; --i) {
            const bool taken = (*step > *(step - budget - 1));
            solution.set(i, taken);
            step -= budget + 1 + taken * instance[i].cost;
        }

        return solution;
    }
};
}  // namespace Knapstack

#endif  // KNAPSTACK_DYNAMIC_PROGRAMMING_HPP