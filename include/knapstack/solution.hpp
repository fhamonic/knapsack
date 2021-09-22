#ifndef KNAPSTACK_SOLUTION_HPP
#define KNAPSTACK_SOLUTION_HPP

#include <vector>

#include "instance.hpp"

namespace fhamonic {
namespace knapstack {

template <typename Value, typename Cost>
class Solution {
private:
    const Instance<Value, Cost> & instance;
    std::vector<bool> _taken;

public:
    Solution(const Instance<Value, Cost> & i)
        : instance(i), _taken(i.itemCount()) {}

    void add(size_t i) { _taken[i] = true; }
    void set(size_t i, bool b) { _taken[i] = b; }
    void remove(size_t i) { _taken[i] = false; }
    bool isTaken(size_t i) { return _taken[i]; }

    auto & operator[](size_t i) { return _taken[i]; }

    Value getValue() const {
        Value sum{};
        for(size_t i = 0; i < instance.itemCount(); ++i)
            if(_taken[i]) sum += instance[i].value;
        return sum;
    }
    Cost getCost() const {
        Cost sum{};
        for(size_t i = 0; i < instance.itemCount(); ++i)
            if(_taken[i]) sum += instance[i].cost;
        return sum;
    }
};

}  // namespace knapstack
}  // namespace fhamonic

#endif  // KNAPSTACK_SOLUTION_HPP