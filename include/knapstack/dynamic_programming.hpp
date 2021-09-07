#ifndef KNAPSTACK_DYNAMIC_PROGRAMMING_HPP
#define KNAPSTACK_DYNAMIC_PROGRAMMING_HPP

#include <numeric>
#include <stack>
#include <type_traits>
#include <vector>

#include <range/v3/algorithm/remove_if.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/view/zip.hpp>

#include "knapstack/instance.hpp"
#include "knapstack/solution.hpp"

namespace Knapstack {
template <typename Value, typename Cost,
          typename std::enable_if<std::is_integral<Cost>::value>::type>
class DynamicProgramming {
public:
    using TInstance = Instance<Value, Cost>;
    using TItem = typename TInstance::Item;
    using TSolution = Solution<Inst, Value, Cost>;

public:
    DynamicProgramming() {}

    TSolution solve(const TInstance & instance) {
        TSolution solution(instance);
        const int nb_items = instance.itemCount();
        const Cost budget = instance.getBudget();

        auto tab = std::make_unique<Value[]>((nb_items + 2) * budget);

        Value * previous_tab = tab.get() + budget;
        for(int w = 0; w < instance.getBudget(); ++w) {
            previous_tab[w] = 0;
        }

        for(const auto & item : instance.getItems()) {
            Value * current_tab = previous_tab + budget;
            for(int w = 0; w < instance.getBudget(); ++w) {
                current_tab[w] =
                    std::max(previous_tab[w],
                             (item.cost > w) *
                                 (previous_tab[w - item.cost] + item.value));
            }
            previous_tab = current_tab;
        }

        return solution;
    }
};
}  // namespace Knapstack

#endif  // KNAPSTACK_DYNAMIC_PROGRAMMING_HPP