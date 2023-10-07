#ifndef UBOUNDED_FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP
#define UBOUNDED_FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP

#include <chrono>
#include <future>
#include <iterator>
#include <numeric>
#include <ranges>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include <range/v3/algorithm/sort.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>

namespace fhamonic {
namespace knapsack {

template <typename C, typename RI, typename VM, typename CM>
class unbounded_knapsack_bnb {
private:
    using I = std::ranges::range_value_t<RI>;
    using V = std::invoke_result_t<VM, I>;

    C _budget;
    std::vector<I> _permuted_items;
    std::vector<std::pair<V, C>> _value_cost_pairs;
    std::vector<std::pair<typename std::vector<std::pair<V, C>>::const_iterator,
                          std::size_t>>
        _best_sol;

private:
    double value_cost_ratio(const std::pair<V, C> & p) const noexcept {
        if constexpr(std::numeric_limits<float>::is_iec559) {
            return p.first / static_cast<double>(p.second);
        } else {
            return (p.second == 0) ? std::numeric_limits<double>::max()
                                   : (p.first / static_cast<double>(p.second));
        }
    }

    auto iterative_bnb() noexcept {
        _best_sol.resize(0);
        auto it = _value_cost_pairs.cbegin();
        const auto end = _value_cost_pairs.cend();
        std::vector<std::pair<decltype(it), std::size_t>> current_sol;
        V current_sol_value = 0;
        V best_sol_value = 0;
        C budget_left = _budget;
        goto begin;
    backtrack:
        while(!current_sol.empty()) {
            it = current_sol.back().first;
            if(--current_sol.back().second == 0) current_sol.pop_back();
            current_sol_value -= it->first;
            budget_left += it->second;
            for(++it; it < end; ++it) {
                if(budget_left < it->second) continue;
                if(current_sol_value + budget_left * value_cost_ratio(*it) <=
                   best_sol_value)
                    goto backtrack;
            begin:
                const std::size_t nb_take =
                    static_cast<std::size_t>(budget_left / it->second);
                current_sol_value += static_cast<V>(nb_take) * it->first;
                budget_left -= static_cast<C>(nb_take) * it->second;
                current_sol.emplace_back(it, nb_take);
            }
            if(current_sol_value <= best_sol_value) continue;
            best_sol_value = current_sol_value;
            _best_sol = current_sol;
        }
        return _best_sol;
    }

    auto iterative_bnb_timeout(std::stop_token stoken) noexcept {
        _best_sol.resize(0);
        auto it = _value_cost_pairs.cbegin();
        const auto end = _value_cost_pairs.cend();
        std::vector<std::pair<decltype(it), std::size_t>> current_sol;
        V current_sol_value = 0;
        V best_sol_value = 0;
        C budget_left = _budget;
        goto begin;
    backtrack:
        while(!current_sol.empty() && !stoken.stop_requested()) {
            it = current_sol.back().first;
            if(--current_sol.back().second == 0) current_sol.pop_back();
            current_sol_value -= it->first;
            budget_left += it->second;
            for(++it; it < end; ++it) {
                if(budget_left < it->second) continue;
                if(current_sol_value + budget_left * value_cost_ratio(*it) <=
                   best_sol_value)
                    goto backtrack;
            begin:
                const std::size_t nb_take =
                    static_cast<std::size_t>(budget_left / it->second);
                current_sol_value += static_cast<V>(nb_take) * it->first;
                budget_left -= static_cast<C>(nb_take) * it->second;
                current_sol.emplace_back(it, nb_take);
            }
            if(current_sol_value <= best_sol_value) continue;
            best_sol_value = current_sol_value;
            _best_sol = current_sol;
        }
        return _best_sol;
    }

public:
    unbounded_knapsack_bnb(const C budget, const RI & items,
                           const VM & value_map, const CM & cost_map) noexcept
        : _budget(budget) {
        if constexpr(std::ranges::sized_range<RI>) {
            _permuted_items.reserve(std::ranges::size(items));
            _value_cost_pairs.reserve(std::ranges::size(items));
        }

        for(auto && i : items) {
            const V value = value_map(i);
            if(value == static_cast<V>(0)) continue;
            const C cost = cost_map(i);
            if(cost > _budget) continue;
            _permuted_items.emplace_back(i);
            _value_cost_pairs.emplace_back(value, cost);
        }

        auto zip_view = ranges::view::zip(_value_cost_pairs, _permuted_items);
        ranges::sort(zip_view, [this](auto p1, auto p2) {
            return value_cost_ratio(p1.first) > value_cost_ratio(p2.first);
        });
    }

    void solve() noexcept { iterative_bnb(); }
    
    template <typename _Rep, typename _Period>
    bool solve(const std::chrono::duration<_Rep, _Period> & timeout) noexcept {
        if(timeout == timeout.zero()) {
            solve();
            return true;
        }
        std::jthread t([this](std::stop_token stoken) {
            return iterative_bnb_timeout(stoken);
        });
        // C++23 should allow to call jthread from future and prevent launching
        // the supplementary thread for join
        auto future = std::async(std::launch::async, &std::jthread::join, &t);
        if(future.wait_for(timeout) == std::future_status::timeout) {
            t.request_stop();
            return false;
        }
        return true;
    }

    auto solution() const noexcept {
        return std::ranges::views::transform(_best_sol, [this](auto & p) {
            return std::make_pair(
                _permuted_items[static_cast<std::size_t>(
                    std::distance(_value_cost_pairs.cbegin(), p.first))],
                p.second);
        });
    }
};

}  // namespace knapsack
}  // namespace fhamonic

#endif  // UBOUNDED_FHAMONIC_KNAPSACK_BRANCH_AND_BOUND_HPP