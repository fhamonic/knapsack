#ifndef FHAMONIC_KNAPSACK_DYNAMIC_PROGRAMMING_HPP
#define FHAMONIC_KNAPSACK_DYNAMIC_PROGRAMMING_HPP

#include <algorithm>
#include <concepts>
#include <numeric>
#include <type_traits>

namespace fhamonic {
namespace knapsack {

template <typename C, typename RI, typename VM, typename CM>
    requires std::integral<C>
class knapsack_dp {
public:
    using I = std::ranges::range_value_t<RI>;
    using V = std::invoke_result_t<VM, I>;

    C _budget;
    std::vector<I> _items;
    std::vector<std::pair<V, C>> _value_cost_pairs;
    std::vector<V> _tab;

public:
    knapsack_dp(const C budget, const RI & items, const VM & value_map,
                const CM & cost_map) noexcept
        : _budget(budget) {
        if constexpr(std::ranges::sized_range<RI>) {
            const std::size_t nb_items = std::ranges::size(items);
            _items.reserve(nb_items);
            _value_cost_pairs.reserve(nb_items);
            _tab.resize((nb_items + 1) * static_cast<std::size_t>(_budget + 1));
        }

        for(auto && i : items) {
            const V value = value_map(i);
            if(value == static_cast<V>(0)) continue;
            const C cost = cost_map(i);
            if(cost > _budget) continue;
            _items.emplace_back(i);
            _value_cost_pairs.emplace_back(value, cost);
        }
    }

    void solve() {
        V * previous_tab = _tab.data();
        for(C w = 0; w < _budget; ++w) {
            previous_tab[w] = 0;
        }

        for(const auto & [value, cost] : _value_cost_pairs) {
            V * const current_tab = previous_tab + _budget + 1;
            C w = std::min(_budget, cost);
            std::copy(previous_tab, previous_tab + w, current_tab);
            for(; w <= _budget; ++w) {
                current_tab[w] =
                    std::max(previous_tab[w], previous_tab[w - cost] + value);
            }
            previous_tab = current_tab;
        }
    }

    auto solution() const noexcept {
        const std::size_t nb_items = _items.size();
        std::vector<I> solution;
        const V * step = _tab.data() + (nb_items * (_budget + 1)) + _budget;

        for(std::size_t i = (nb_items - 1); i > 0; --i) {
            const bool taken = (*step > *(step - _budget - 1));
            if(taken) solution.push_back(_items[i]);
            step -= _budget + 1 + taken * _value_cost_pairs[i].second;
        }
        const bool taken = (*step > *(step - _budget - 1));
        if(taken) solution.push_back(_items[0]);

        return solution;
    }
};

}  // namespace knapsack
}  // namespace fhamonic

#endif  // FHAMONIC_KNAPSACK_DYNAMIC_PROGRAMMING_HPP