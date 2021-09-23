#ifndef UNBOUNDED_KNAPSTACK_SOLUTION_HPP
#define UNBOUNDED_KNAPSTACK_SOLUTION_HPP

#include <vector>

#include "unbounded_knapsack/instance.hpp"

namespace fhamonic {
namespace unbounded_knapsack {

template <typename Value, typename Cost>
class Solution {
private:
    const Instance<Value, Cost> & instance;
    std::vector<int> _nb_taken;

public:
    Solution(const Instance<Value, Cost> & i)
        : instance(i), _nb_taken(i.itemCount()) {}

    void add(size_t i) { ++_nb_taken[i]; }
    void set(size_t i, int n) { _nb_taken[i] = n; }
    void remove(size_t i) { _nb_taken[i] = 0; }
    bool isTaken(size_t i) { return _nb_taken[i] > 0; }

    int & operator[](size_t i) { return _nb_taken[i]; }

    Value getValue() const {
        Value sum{};
        for(size_t i = 0; i < instance.itemCount(); ++i)
            sum += _nb_taken[i] * instance[i].value;
        return sum;
    }
    Cost getCost() const {
        Cost sum{};
        for(size_t i = 0; i < instance.itemCount(); ++i)
            sum += _nb_taken[i] * instance[i].cost;
        return sum;
    }
};

}  // namespace unbounded_knapsack
}  // namespace fhamonic

#endif  // UNBOUNDED_KNAPSTACK_SOLUTION_HPP