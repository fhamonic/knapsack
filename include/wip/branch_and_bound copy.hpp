#ifndef BRANCH_AND_BOUND_HPP
#define BRANCH_AND_BOUND_HPP

#include <numeric>
#include <vector>

#include <range/v3/algorithm/sort.hpp>
#include <range/v3/view/zip.hpp>

#include "instance.hpp"
#include "solution.hpp"

namespace Knapsack {
template <template <typename, typename> class Inst, typename Value,
          typename Cost>
class BranchAndBound {
public:
    using TInstance = Inst<Value, Cost>;
    using TItem = typename TInstance::Item;
    using TSolution = Solution<Inst, Value, Cost>;

private:
    const TInstance & instance;
    std::vector<TItem> sorted_items;

    std::vector<bool> best_takens;
    Value best_value;

    std::vector<bool> takens;

    double computeUpperBound(size_t depth, Value bound_value,
                             Cost bound_budget_left) {
        for(; depth < sorted_items.size(); ++depth) {
            const TItem & item = sorted_items[depth];
            if(bound_budget_left <= item.cost)
                return bound_value + bound_budget_left * item.getRatio();
            bound_budget_left -= item.cost;
            bound_value += item.value;
        }
        return bound_value;
    }

    void recursive_bnb(const size_t depth, Value value, Cost budget_left) {
        if(depth == sorted_items.size()) {  // leaf
            if(value > best_value) {
                best_value = value;
                best_takens = takens;
            }
            return;
        }
        const TItem & item = sorted_items[depth];
        if(item.cost <= budget_left) {
            takens[depth] = true;
            recursive_bnb(depth + 1, value + item.value,
                          budget_left - item.cost);
            takens[depth] = false;
        }
        if(computeUpperBound(depth, value, budget_left) <= best_value)
            return;  // this node could not be in a better solution
        recursive_bnb(depth + 1, value, budget_left);
    }

public:
    BranchAndBound(TInstance & instance)
        : instance(instance), best_takens(instance.itemCount(), false) {}

    TSolution solve() {
        sorted_items = instance.getItems();
        std::vector<int> permuted_id(instance.itemCount());
        std::iota(permuted_id.begin(), permuted_id.end(), 0);

        auto zip_view = ranges::view::zip(sorted_items, permuted_id);
        ranges::sort(zip_view,
                     [](auto p1, auto p2) { return p1.first < p2.first; });

        best_value = 0;
        takens.clear();
        takens.resize(instance.itemCount(), false);
        recursive_bnb(0, 0, instance.getBudget());

        TSolution solution(instance);
        for(size_t i = 0; i < instance.itemCount(); ++i) {
            if(best_takens[i]) solution.add(permuted_id[i]);
        }
        return solution;
    }
};
}  // namespace Knapsack

#endif  // BRANCH_AND_BOUND_HPP