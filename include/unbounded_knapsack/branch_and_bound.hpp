#ifndef UBOUNDED_FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP
#define UBOUNDED_FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP

#include <numeric>
#include <stack>
#include <vector>

#include <range/v3/algorithm/remove_if.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/view/zip.hpp>

#include "unbounded_knapsack/instance.hpp"
#include "unbounded_knapsack/solution.hpp"

namespace fhamonic {
namespace unbounded_knapsack {

template <typename Value, typename Cost>
class BranchAndBound {
public:
    using TInstance = Instance<Value, Cost>;
    using TItem = typename TInstance::Item;
    using TSolution = Solution<Value, Cost>;

private:
    double computeUpperBound(const std::vector<TItem> & sorted_items,
                             size_t depth, Value bound_value,
                             Cost bound_budget_left) {
        for(; depth < sorted_items.size(); ++depth) {
            const TItem & item = sorted_items[depth];
            if(bound_budget_left <= item.cost)
                return bound_value + bound_budget_left * item.getRatio();
            const int nb_take = bound_budget_left / item.cost;
            bound_budget_left -= nb_take * item.cost;
            bound_value += nb_take * item.value;
        }
        return bound_value;
    }

    std::stack<std::pair<int, int>> iterative_bnb(
        const std::vector<TItem> & sorted_items, Cost budget_left) {
        const int nb_items = sorted_items.size();
        int depth = 0;
        Value value = 0;
        Value best_value = 0;
        std::stack<std::pair<int, int>> stack;
        std::stack<std::pair<int, int>> best_stack;
        goto begin;
    backtrack:
        while(!stack.empty()) {
            depth = stack.top().first;
            if(--stack.top().second == 0) stack.pop();
            value -= sorted_items[depth].value;
            budget_left += sorted_items[depth++].cost;
            for(; depth < nb_items; ++depth) {
                if(budget_left < sorted_items[depth].cost) continue;
                if(computeUpperBound(sorted_items, depth, value, budget_left) <=
                   best_value)
                    goto backtrack;
            begin:
                const int nb_take = budget_left / sorted_items[depth].cost;
                value += nb_take * sorted_items[depth].value;
                budget_left -= nb_take * sorted_items[depth].cost;
                stack.emplace(depth, nb_take);
            }
            if(value <= best_value) continue;
            best_value = value;
            best_stack = stack;
        }
        return best_stack;
    }

public:
    BranchAndBound() {}

    TSolution solve(const TInstance & instance) {
        std::vector<TItem> sorted_items = instance.getItems();
        std::vector<int> permuted_id(instance.itemCount());
        std::iota(permuted_id.begin(), permuted_id.end(), 0);

        auto zip_view = ranges::view::zip(sorted_items, permuted_id);
        auto end = ranges::remove_if(zip_view, [&](const auto & r) {
            return r.first.cost > instance.getBudget();
        });
        const ptrdiff_t new_size = std::distance(zip_view.begin(), end);
        sorted_items.erase(sorted_items.begin() + new_size, sorted_items.end());
        permuted_id.erase(permuted_id.begin() + new_size, permuted_id.end());
        ranges::sort(zip_view,
                     [](auto p1, auto p2) { return p1.first < p2.first; });

        std::stack<std::pair<int, int>> best_stack =
            iterative_bnb(sorted_items, instance.getBudget());

        TSolution solution(instance);
        while(!best_stack.empty()) {
            solution.set(permuted_id[best_stack.top().first],
                         best_stack.top().second);
            best_stack.pop();
        }
        return solution;
    }
};

}  // namespace unbounded_knapsack
}  // namespace fhamonic

#endif  // UBOUNDED_FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP