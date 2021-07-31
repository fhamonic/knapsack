#ifndef PARALLEL_BRANCH_AND_BOUND_HPP
#define PARALLEL_BRANCH_AND_BOUND_HPP

#include <atomic>
#include <mutex>
#include <numeric>
#include <vector>

#include <range/v3/view/zip.hpp>
#include <range/v3/algorithm/sort.hpp>

#include "instance.hpp"
#include "solution.hpp"

namespace Knapstack {
    template <template<typename,typename> class Inst, typename Value, typename Cost>
    class ParallelBranchAndBound {
    public:
        using TInstance = Inst<Value,Cost>;
        using TItem = typename TInstance::Item;
        using TSolution = Solution<Inst, Value, Cost>;
    private:
        const TInstance & instance;
        std::vector<TItem> sorted_items;

        std::mutex solution_mutex;
        std::vector<bool> best_takens;
        std::atomic<Value> best_value;
        
        struct Node {
            std::reference_wrapper<BranchAndBound> solver;
            std::vector<bool> takens;
            Value value;
            Cost budget_left;
            size_t depth;

            Node(const TInstance & instance, BranchAndBound & solver)
                : solver(solver)
                , takens(instance.itemCount(), false)
                , value{0}
                , budget_left{instance.getBudget()}
                , depth{0} {}
            
            Node(const Node & n) = default;

            double computeUpperBound() {
                Value bound_value = value;
                Cost bound_budget_left = budget_left;            
                for(size_t i=depth; i<solver.get().instance.itemCount(); ++i) {
                    const TItem & item = solver.get().sorted_items[i];
                    if(bound_budget_left <= item.cost)
                        return bound_value + bound_budget_left * item.getRatio();
                    bound_budget_left -= item.cost;
                    bound_value += item.value;
                }
                return bound_value;
            }

            void recursive_bnb() {
                if(depth == solver.get().instance.itemCount()) { // leaf
                    if(value > solver.get().best_value) {
                        solver.get().best_value = value;
                        solver.get().solution_mutex.lock();
                        solver.get().best_takens = takens;
                        solver.get().solution_mutex.unlock();
                    }
                    return;
                }
                if(computeUpperBound() <= solver.get().best_value)
                    return; // this node could not be in a better solution
                const TItem & item = solver.get().sorted_items[depth];
                if(item.cost <= budget_left) {
                    takens[depth++] = true;
                    value += item.value;
                    budget_left -= item.cost;
                    recursive_bnb();
                    takens[--depth] = false;
                    value -= item.value;
                    budget_left += item.cost;
                }
                ++depth;
                recursive_bnb();
                --depth;
            }
        };
    public:
        BranchAndBound(TInstance & instance)
            : instance(instance)
            , best_takens(instance.itemCount(), false) {}
    
        TSolution solve() {
            sorted_items = instance.getItems();
            std::vector<int> permuted_id(instance.itemCount());
            std::iota(permuted_id.begin(), permuted_id.end(), 0);

            auto zip_view = ranges::view::zip(sorted_items, permuted_id);
            ranges::sort(zip_view, [](auto p1, auto p2){ return p1.first < p2.first; });
            
            best_value = 0;
            Node n(instance, *this);
            n.recursive_bnb();

            TSolution solution(instance);
            for(size_t i=0; i<instance.itemCount(); ++i) {
                if(best_takens[i])
                    solution.add(permuted_id[i]);
            }

            return solution;
        }
    };
} //namespace Knapstack

#endif //PARALLEL_BRANCH_AND_BOUND_HPP