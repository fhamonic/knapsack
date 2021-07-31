#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <vector>

#include "instance.hpp"

namespace Knapstack {
    template <template<typename,typename> class TInstance, typename Value, typename Cost>
    class Solution {
    private:
        const TInstance<Value,Cost> & instance;
        std::vector<bool> _taken;
    public:
        Solution(const TInstance<Value,Cost> & i) : instance(i), _taken(i.itemCount()) {}

        void add(size_t i) { _taken[i] = true; }
        void remove(size_t i) { _taken[i] = false; }
        bool isTaken(size_t i) { return _taken[i]; }

        Value getValue() const {
            Value sum{};
            for(size_t i=0; i<instance.itemCount(); ++i)
                if(_taken[i]) sum += instance[i].value;
            return sum;
        }
        Cost getCost() const { 
            Cost sum{};
            for(size_t i=0; i<instance.itemCount(); ++i)
                if(_taken[i]) sum += instance[i].cost;
            return sum;
        }
    };
} //namespace Knapstack

#endif //SOLUTION_HPP