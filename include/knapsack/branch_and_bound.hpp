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
        double computeUpperBound(const std::vector<TItem> & sorted_items,
                             std::size_t depth, Value bound_value,
                             Cost bound_budget_left) {
        for(; depth < sorted_items.size(); ++depth) {
            const TItem & item = sorted_items[depth];
            if(bound_budget_left < item.cost)
                return bound_value + bound_budget_left * item.getRatio();
            bound_budget_left -= item.cost;
            bound_value += item.value;
        }
        return bound_value;
    }

    std::vector<std::size_t> iterative_bnb(
        const std::vector<TItem> & sorted_items, Cost budget_left) {
        const std::size_t nb_items = sorted_items.size();
        std::size_t depth = 0;
        Value value = 0;
        Value best_value = 0;
        std::vector<std::size_t> stack;
        std::vector<std::size_t> best_stack;
        goto begin;
    backtrack:
        while(!stack.empty()) {
            depth = stack.back();
            stack.pop_back();
            value -= sorted_items[depth].value;
            budget_left += sorted_items[depth].cost;
            for(++depth; depth < nb_items; ++depth) {
                if(budget_left < sorted_items[depth].cost) continue;
                if(computeUpperBound(sorted_items, depth, value, budget_left) <=
                   best_value)
                    goto backtrack;
            begin:
                value += sorted_items[depth].value;
                budget_left -= sorted_items[depth].cost;
                stack.push_back(depth);
            }
            if(value <= best_value) continue;
            best_value = value;
            best_stack = stack;
        }
        return best_stack;
    }


    // double computeUpperBound(const std::vector<TItem> & sorted_items,
    //                          auto it, Value bound_value,
    //                          Cost bound_budget_left) {
    //     const auto end = sorted_items.cend();
    // for(; it < end; ++it) {
    //         if(bound_budget_left < it->cost)
    //             return bound_value + bound_budget_left * it->getRatio();
    //         bound_budget_left -= it->cost;
    //         bound_value += it->value;
    //     }
        
    //     return bound_value;
    // }

    // auto iterative_bnb(const std::vector<TItem> & sorted_items,
    //                                  Cost budget_left) {
    //     Value value = 0;
    //     Value best_value = 0;
    //     std::vector<typename std::vector<TItem>::const_iterator> stack;
    //     std::vector<typename std::vector<TItem>::const_iterator> best_stack;
    //     auto it = sorted_items.cbegin();
    //     const auto end = sorted_items.cend();
    //     goto begin;
    // backtrack:
    //     while(!stack.empty()) {
    //         it = stack.back();
    //         stack.pop_back();
    //         value -= it->value;
    //         budget_left += it->cost;
    //         for(++it; it < end; ++it) {
    //             if(budget_left < it->cost)
    //                 continue;
    //             if(computeUpperBound(sorted_items, it, value,
    //                                  budget_left) <= best_value)
    //                 goto backtrack;
    //         begin:
    //             value += it->value;
    //             budget_left -= it->cost;
    //             stack.push_back(it);
    //         }
    //         if(value <= best_value) continue;
    //         best_value = value;
    //         best_stack = stack;
    //     }
    //     return best_stack;
    // }

public:
    BranchAndBound() {}

    TSolution solve(const TInstance & instance) {
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


            auto best_stack =
                iterative_bnb(sorted_items, instance.getBudget());
            for(std::size_t i : best_stack) {
                solution.add(permuted_id[i]); 
            }


            // auto best_stack = iterative_bnb(sorted_items, instance.getBudget());
            // for(auto && it : best_stack) {
            //     solution.add(permuted_id[static_cast<std::size_t>(std::distance(sorted_items.cbegin(), it))]);
            // }
        }
        return solution;
    }
};
}  // namespace knapsack
}  // namespace fhamonic

#endif  // FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP