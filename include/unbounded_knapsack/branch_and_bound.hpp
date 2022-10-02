#ifndef UBOUNDED_FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP
#define UBOUNDED_FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP

#include <numeric>
#include <stack>
#include <vector>

#include <range/v3/algorithm/remove_if.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/view/drop.hpp>
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
    double value_cost_ratio(const TItem & p) const noexcept {
        if constexpr(std::numeric_limits<float>::is_iec559) {
            return p.value / static_cast<double>(p.cost);
        } else {
            return (p.cost == 0) ? std::numeric_limits<double>::max()
                                 : (p.value / static_cast<double>(p.cost));
        }
    }

    auto iterative_bnb(const std::vector<TItem> & sorted_items,
                       Cost budget_left) const noexcept {
        Value current_sol_value = 0;
        Value best_sol_value = 0;
        auto it = sorted_items.cbegin();
        const auto end = sorted_items.cend();
        std::vector<std::pair<decltype(it), std::size_t>> current_sol;
        std::vector<std::pair<decltype(it), std::size_t>> best_sol;
        goto begin;
    backtrack:
        while(!current_sol.empty()) {
            it = current_sol.back().first;
            if(--current_sol.back().second == 0) current_sol.pop_back();
            current_sol_value -= it->value;
            budget_left += it->cost;
            for(++it; it < end; ++it) {
                if(budget_left < it->cost) continue;
                if(current_sol_value + budget_left * value_cost_ratio(*it) <=
                   best_sol_value)
                    goto backtrack;
            begin:
                const std::size_t nb_take =
                    static_cast<std::size_t>(budget_left / it->cost);
                current_sol_value += static_cast<Value>(nb_take) * it->value;
                budget_left -= static_cast<Cost>(nb_take) * it->cost;
                current_sol.emplace_back(it, nb_take);
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
        std::vector<TItem> sorted_items = instance.getItems();
        std::vector<std::size_t> permuted_id(instance.itemCount());
        std::iota(permuted_id.begin(), permuted_id.end(), 0);

        auto zip_view = ranges::view::zip(sorted_items, permuted_id);
        auto end = ranges::remove_if(zip_view, [&](const auto & r) {
            return r.first.cost > instance.getBudget();
        });
        const ptrdiff_t new_size = std::distance(zip_view.begin(), end);
        sorted_items.erase(sorted_items.begin() + new_size, sorted_items.end());
        permuted_id.erase(permuted_id.begin() + new_size, permuted_id.end());
        ranges::sort(zip_view, [this](auto p1, auto p2) {
            return value_cost_ratio(p1.first) > value_cost_ratio(p2.first);
        });

        TSolution solution(instance);

        auto best_sol = iterative_bnb(sorted_items, instance.getBudget());
        for(auto && [it, nb] : best_sol) {
            solution.set(permuted_id[static_cast<std::size_t>(
                             std::distance(sorted_items.cbegin(), it))],
                         nb);
        }

        return solution;
    }
};

}  // namespace unbounded_knapsack
}  // namespace fhamonic

#endif  // UBOUNDED_FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP