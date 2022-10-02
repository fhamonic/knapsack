#ifndef UNBOUNDED_FHAMONIC_KNAPSACK_SOLUTION_HPP
#define UNBOUNDED_FHAMONIC_KNAPSACK_SOLUTION_HPP

#include <vector>

#include "unbounded_knapsack/instance.hpp"

namespace fhamonic {
namespace unbounded_knapsack {

template <typename Value, typename Cost>
class Solution {
private:
    const Instance<Value, Cost> & instance;
    std::vector<std::size_t> _nb_taken;

public:
    Solution(const Instance<Value, Cost> & i)
        : instance(i), _nb_taken(i.itemCount()) {}

    void add(std::size_t i) { ++_nb_taken[i]; }
    void set(std::size_t i, std::size_t n) { _nb_taken[i] = n; }
    void remove(std::size_t i) { _nb_taken[i] = 0; }
    bool isTaken(std::size_t i) { return _nb_taken[i] > 0; }

    std::size_t & operator[](std::size_t i) { return _nb_taken[i]; }

    Value getValue() const {
        Value sum{};
        for(std::size_t i = 0; i < instance.itemCount(); ++i)
            sum += static_cast<Value>(_nb_taken[i]) * instance[i].value;
        return sum;
    }
    Cost getCost() const {
        Cost sum{};
        for(std::size_t i = 0; i < instance.itemCount(); ++i)
            sum += static_cast<Cost>(_nb_taken[i]) * instance[i].cost;
        return sum;
    }
};

}  // namespace unbounded_knapsack
}  // namespace fhamonic

#endif  // UNBOUNDED_FHAMONIC_KNAPSACK_SOLUTION_HPP