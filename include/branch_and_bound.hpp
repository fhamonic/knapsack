#ifndef BRANCH_AND_BOUND_HPP
#define BRANCH_AND_BOUND_HPP

#include <numeric>
#include <vector>

#include <range/v3/view/zip.hpp>
#include <range/v3/algorithm/sort.hpp>

#include "instance.hpp"
#include "solution.hpp"

namespace Knapstack {
    template <template<typename,typename> class Inst, typename Value, typename Cost>
    class BranchAndBound {
    public:
        using TInstance = Inst<Value,Cost>;
        using TItem = typename TInstance::Item;
        using TSolution = Solution<Inst, Value, Cost>;
    private:
        const TInstance & instance;
        std::vector<TItem> sorted_items;

        std::vector<bool> best_takens;
        Value best_value;

        double computeUpperBound(size_t depth, Value bound_value, Cost bound_budget_left) { 
            for(; depth<sorted_items.size(); ++depth) {
                const TItem & item = sorted_items[depth];
                if(bound_budget_left <= item.cost)
                    return bound_value + bound_budget_left * item.getRatio();
                bound_budget_left -= item.cost;
                bound_value += item.value;
            }
            return bound_value;
        }

        void iterative_bnb() {
            const int nb_items = sorted_items.size();
            Value value = 0;
            Cost budget_left = instance.getBudget();
            int depth = 0;
            std::vector<bool> takens(nb_items, false);
            goto begin;
            for(;;) {
            backtrack:
                for(--depth; depth>=0 && !takens[depth]; --depth);
                if(depth < 0) return;
                value -= sorted_items[depth].value;
                budget_left += sorted_items[depth].cost;
                takens[depth++] = false;
            begin:
                for(; depth<nb_items; ++depth) {
                    const TItem & add_item = sorted_items[depth];
                    if(budget_left < add_item.cost) continue;
                    if(computeUpperBound(depth, value, budget_left) <= best_value)
                        goto backtrack;
                    value += add_item.value;
                    budget_left -= add_item.cost;
                    takens[depth] = true;
                }
                if(value <= best_value) 
                    continue;
                best_value = value;
                best_takens = takens;
            }
        }
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
            iterative_bnb();

            TSolution solution(instance);
            for(size_t i=0; i<instance.itemCount(); ++i) {
                if(best_takens[i])
                    solution.add(permuted_id[i]);
            }
            return solution;
        }
    };
} //namespace Knapstack

#endif //BRANCH_AND_BOUND_HPP