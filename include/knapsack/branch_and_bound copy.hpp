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

template <typename V, typename C>
class knapsack_bnb {
private:
    double value_cost_ratio(const std::pair<V, C> & p) {
        if constexpr(std::numeric_limits<float>::is_iec559()) {
            return p.first / p.second;
        } else {
            return (p.second == 0) ? std::numeric_limits<double>::max()
                                   : (p.first / p.second);
        }
    }

    V computeUpperBound(auto it, const auto end, V bound_value,
                        C bound_budget_left) const noexcept {
        for(; it < end; ++it) {
            if(bound_budget_left < it->cost)
                return static_cast<V>(bound_value +
                                      bound_budget_left * it->value /
                                          static_cast<double>(it->cost));
            bound_budget_left -= it->cost;
            bound_value += it->value;
        }

        return bound_value;
    }

    auto iterative_bnb(const std::vector<std::pair<V, C>> & value_cost_pairs,
                       C budget_left) const noexcept {
        V current_sol_value = 0;
        V best_sol_value = 0;
        auto it = value_cost_pairs.cbegin();
        const auto end = value_cost_pairs.cend();
        std::vector<decltype(it)> current_sol;
        std::vector<decltype(it)> best_sol;
        if(it == end) return best_sol;
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
    knapsack_bnb() {}

    template <typename I, typename RI, typenmae VM, typename CM>
    auto solve(const C budget, const RI & items, const const VM & value_map,
               const CM cost_map) const noexcept {
        std::vector<std::pair<V, C>> value_cost_pairs =
            ranges::to<std::vector<std::pair<V, C>>>(
                ranges::transform(items, [&value_map, &cost_map](auto && i) {
                    return std::make_pair(value_map(i), cost_map(i));
                }));

        std::vector<I> permuted_items = ranges::to<std::vector<I>>(items);

        auto zip_view = ranges::view::zip(value_cost_pairs, permuted_items);
        auto end = ranges::remove_if(zip_view, [budget](const auto & e) {
            return e.first.second > budget;
        });
        const auto new_size = std::distance(zip_view.begin(), end);
        value_cost_pairs.erase(value_cost_pairs.begin() + new_size,
                               value_cost_pairs.end());
        permuted_items.erase(permuted_items.begin() + new_size,
                             permuted_items.end());
        ranges::sort(zip_view, [](auto p1, auto p2) {
            return value_cost_ratio(p1) > value_cost_ratio(p2);
        });

        auto best_sol = iterative_bnb(value_cost_pairs, budget);

        return ranges::transform(best_sol, [&permuted_items](auto && it) {
            return permuted_items[static_cast<std::size_t>(
                std::distance(value_cost_pairs.cbegin(), it))];
        });
    }
};
}  // namespace knapsack
}  // namespace fhamonic

#endif  // FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP