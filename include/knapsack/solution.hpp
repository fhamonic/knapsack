#ifndef FHAMONIC_KNAPSACK_SOLUTION_HPP
#define FHAMONIC_KNAPSACK_SOLUTION_HPP

#include <vector>

#include "instance.hpp"

namespace fhamonic {
namespace knapsack {

template <typename Value, typename Cost>
class Solution {
private:
    const Instance<Value, Cost> & instance;
    std::vector<bool> _taken;

public:
    Solution(const Instance<Value, Cost> & i)
        : instance(i), _taken(i.itemCount()) {}

    void add(const std::size_t i) { _taken[i] = true; }
    void set(const std::size_t i, bool b) { _taken[i] = b; }
    void remove(const std::size_t i) { _taken[i] = false; }
    bool isTaken(const std::size_t i) { return _taken[static_cast<std::size_t>(i)]; }

    auto & operator[](const std::size_t i) { return _taken[i]; }

    Value getValue() const {
        Value sum{};
        for(std::size_t i = 0; i < instance.itemCount(); ++i)
            if(_taken[i]) sum += instance[i].value;
        return sum;
    }
    Cost getCost() const {
        Cost sum{};
        for(std::size_t i = 0; i < instance.itemCount(); ++i)
            if(_taken[i]) sum += instance[i].cost;
        return sum;
    }
};

}  // namespace knapsack
}  // namespace fhamonic

#endif  // FHAMONIC_KNAPSACK_SOLUTION_HPP