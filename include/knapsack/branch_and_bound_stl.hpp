#ifndef FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP
#define FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP

#include <iterator>
#include <numeric>
#include <ranges>
#include <stack>
#include <type_traits>
#include <utility>
#include <vector>

#include <range/v3/algorithm/remove_if.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

#include "knapsack/instance.hpp"
#include "knapsack/solution.hpp"

namespace fhamonic {
namespace knapsack {

template <typename C, typename RI, typename VM, typename CM>
class knapsack_bnb {
private:
    using I = std::ranges::range_value_t<RI>;
    using V = std::invoke_result_t<VM, I>;

    std::vector<I> permuted_items;
    std::vector<std::pair<V, C>> value_cost_pairs;
    std::vector<typename std::vector<std::pair<V, C>>::const_iterator> best_sol;

private:
    double value_cost_ratio(const std::pair<V, C> & p) const noexcept {
        if constexpr(std::numeric_limits<float>::is_iec559) {
            return p.first / static_cast<double>(p.second);
        } else {
            return (p.second == 0) ? std::numeric_limits<double>::max()
                                   : (p.first / static_cast<double>(p.second));
        }
    }

    V computeUpperBound(auto it, const auto end, V bound_value,
                        C bound_budget_left) const noexcept {
        for(; it < end; ++it) {
            if(bound_budget_left < it->second)
                return static_cast<V>(bound_value +
                                      bound_budget_left * it->first /
                                          static_cast<double>(it->second));
            bound_budget_left -= it->second;
            bound_value += it->first;
        }

        return bound_value;
    }

    void iterative_bnb(C budget_left) noexcept {
        V current_sol_value = 0;
        V best_sol_value = 0;
        auto it = value_cost_pairs.cbegin();
        const auto end = value_cost_pairs.cend();
        std::vector<decltype(it)> current_sol;
        best_sol.clear();
        if(it == end) return;
        goto begin;
    backtrack:
        while(!current_sol.empty()) {
            it = current_sol.back();
            current_sol_value -= it->first;
            budget_left += it->second;
            current_sol.pop_back();
            for(++it; it < end; ++it) {
                if(budget_left < it->second) continue;
                if(computeUpperBound(it, end, current_sol_value, budget_left) <=
                   best_sol_value)
                    goto backtrack;
            begin:
                current_sol_value += it->first;
                budget_left -= it->second;
                current_sol.push_back(it);
            }
            if(current_sol_value <= best_sol_value) continue;
            best_sol_value = current_sol_value;
            best_sol = current_sol;
        }
    }

public:
    knapsack_bnb(const C budget, const RI & items, const VM & value_map,
                 const CM & cost_map) noexcept {
        if constexpr(std::ranges::sized_range<RI>) {
            permuted_items.reserve(std::ranges::size(items));
            value_cost_pairs.reserve(std::ranges::size(items));
        }

        for(auto && i : items) {
            const V value = value_map(i);
            if(value == static_cast<V>(0)) continue;
            const C cost = cost_map(i);
            if(cost > budget) continue;
            permuted_items.emplace_back(i);
            value_cost_pairs.emplace_back(value, cost);
        }

        auto zip_view = ranges::view::zip(value_cost_pairs, permuted_items);
        ranges::sort(zip_view, [this](auto p1, auto p2) {
            return value_cost_ratio(p1.first) > value_cost_ratio(p2.first);
        });
    }

    void solve(C budget) noexcept {
        iterative_bnb(budget);
    }

    auto solution() const noexcept {
        return std::ranges::views::transform(best_sol, [this](auto && it) {
            return permuted_items[static_cast<std::size_t>(
                std::distance(value_cost_pairs.cbegin(), it))];
        });
    }
};
}  // namespace knapsack
}  // namespace fhamonic

#endif  // FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP