#ifndef FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP
#define FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP

#include <iterator>
#include <numeric>
#include <stack>
#include <type_traits>
#include <vector>

#include <range/v3/algorithm/remove_if.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/zip.hpp>

#include "knapsack/instance.hpp"
#include "knapsack/solution.hpp"

namespace fhamonic {
namespace knapsack {

template <typename Value, typename Cost>
class BranchAndBound {
public:
    using TInstance = Instance<Value, Cost>;
    using TItem = typename TInstance::Item;
    using TSolution = Solution<Value, Cost>;

private:
    Value computeUpperBound(auto it, const auto end, Value bound_value,
                             Cost bound_budget_left) const noexcept {
        for(; it < end; ++it) {
            if(bound_budget_left < it->cost)
                return static_cast<Value>(bound_value + bound_budget_left *
                           it->value / static_cast<double>(it->cost));
            bound_budget_left -= it->cost;
            bound_value += it->value;
        }

        return bound_value;
    }

    auto iterative_bnb(const std::vector<TItem> & sorted_items,
                       Cost budget_left) const noexcept {
        Value current_sol_value = 0;
        Value best_sol_value = 0;
        auto it = sorted_items.cbegin();
        const auto end = sorted_items.cend();
        std::vector<decltype(it)> current_sol;
        std::vector<decltype(it)> best_sol;
        goto begin;
    backtrack:
        while(!current_sol.empty()) {
            it = current_sol.back();
            current_sol_value -= it->value;
            budget_left += it->cost;
            current_sol.pop_back();
            for(++it; it < end; ++it) {
                if(budget_left < it->cost) continue;
                if(computeUpperBound(it, end, current_sol_value, budget_left) <=
                   best_sol_value)
                    goto backtrack;
            begin:
                current_sol_value += it->value;
                budget_left -= it->cost;
                current_sol.push_back(it);
            }
            if(current_sol_value <= best_sol_value) continue;
            best_sol_value = current_sol_value;
            best_sol = current_sol;
        }
        return best_sol;
    }

public:
    BranchAndBound() {}

    TSolution solve(const TInstance & instance) const noexcept {
        TSolution solution(instance);
        if(instance.itemCount() > 0) {
            std::vector<TItem> sorted_items = instance.getItems();
            std::vector<std::size_t> permuted_id(instance.itemCount());
            std::iota(permuted_id.begin(), permuted_id.end(), 0);

            auto zip_view = ranges::view::zip(sorted_items, permuted_id);
            auto end = ranges::remove_if(zip_view, [&](const auto & r) {
                return r.first.cost > instance.getBudget();
            });
            const ptrdiff_t new_size = std::distance(zip_view.begin(), end);
            sorted_items.erase(sorted_items.begin() + new_size,
                               sorted_items.end());
            permuted_id.erase(permuted_id.begin() + new_size,
                              permuted_id.end());
            ranges::sort(zip_view,
                         [](auto p1, auto p2) { return p1.first < p2.first; });

            // auto best_sol =
            //     iterative_bnb(sorted_items, instance.getBudget());
            // for(std::size_t i : best_sol) {
            //     solution.add(permuted_id[i]);
            // }

            auto best_sol = iterative_bnb(sorted_items, instance.getBudget());
            for(auto && it : best_sol) {
                solution.add(permuted_id[static_cast<std::size_t>(
                    std::distance(sorted_items.cbegin(), it))]);
            }
        }
        return solution;
    }
};
}  // namespace knapsack
}  // namespace fhamonic

#endif  // FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP