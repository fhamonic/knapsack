#ifndef PARALLEL_BRANCH_AND_BOUND_HPP
#define PARALLEL_BRANCH_AND_BOUND_HPP

#include <atomic>
#include <mutex>
#include <numeric>
#include <thread>
#include <vector>

#include <tbb/concurrent_queue.h>

#include <range/v3/algorithm/sort.hpp>
#include <range/v3/view/zip.hpp>

#include "instance.hpp"
#include "solution.hpp"

namespace Knapstack {
template <template <typename, typename> class Inst, typename Value,
          typename Cost>
class ParallelBranchAndBound {
public:
    using TInstance = Inst<Value, Cost>;
    using TItem = typename TInstance::Item;
    using TSolution = Solution<Inst, Value, Cost>;

private:
    const TInstance & instance;
    std::vector<TItem> sorted_items;

    std::atomic<int> available_threads;
    tbb::concurrent_queue<std::thread> threads;

    std::mutex solution_mutex;
    std::stack<int> best_stack;
    std::atomic<Value> best_value;

    struct Node {
        ParallelBranchAndBound * solver;
        std::stack<int> stack;
        Value value;
        Cost budget_left;
        int depth;

        Node(ParallelBranchAndBound * solver)
            : solver(solver)
            , value{0}
            , budget_left{solver->instance.getBudget()}
            , depth{0} {}

        Node() = default;
        Node(const Node & n) = default;
        Node(Node && n) = default;

        Node & operator=(const Node & n) = default;
        Node & operator=(Node && n) = default;

        double computeUpperBound() {
            Value bound_value = value;
            Cost bound_budget_left = budget_left;
            for(size_t i = depth; i < solver->sorted_items.size(); ++i) {
                const TItem & item = solver->sorted_items[i];
                if(bound_budget_left <= item.cost)
                    return bound_value + bound_budget_left * item.getRatio();
                bound_budget_left -= item.cost;
                bound_value += item.value;
            }
            return bound_value;
        }
    };

    static void iterative_bnb(Node n) {
        goto begin;
    backtrack:
        while(!n.stack.empty()) {
            n.depth = n.stack.top();
            n.stack.pop();
            n.value -= n.solver->sorted_items[n.depth].value;
            n.budget_left += n.solver->sorted_items[n.depth++].cost;
            for(; n.depth < n.solver->sorted_items.size(); ++n.depth) {
                if(n.budget_left < n.solver->sorted_items[n.depth].cost)
                    continue;
                if(n.computeUpperBound() <= n.solver->best_value)
                    goto backtrack;
            begin:
                if(--n.solver->available_threads > 0) {
                    n.solver->threads.emplace(iterative_bnb, n);
                    continue;
                } else {
                    ++n.solver->available_threads;
                }
                n.value += n.solver->sorted_items[n.depth].value;
                n.budget_left -= n.solver->sorted_items[n.depth].cost;
                n.stack.push(n.depth);
            }
            if(n.value <= n.solver->best_value) continue;
            n.solver->best_value = n.value;
            n.solver->solution_mutex.lock();
            n.solver->best_stack = n.stack;
            n.solver->solution_mutex.unlock();
        }
        ++n.solver->available_threads;
    }

public:
    ParallelBranchAndBound(TInstance & instance) : instance(instance) {}

    TSolution solve() {
        sorted_items = instance.getItems();
        std::vector<int> permuted_id(instance.itemCount());
        std::iota(permuted_id.begin(), permuted_id.end(), 0);

        auto zip_view = ranges::view::zip(sorted_items, permuted_id);
        auto end = ranges::remove_if(zip_view, [&](const auto & r) {
            return r.first.cost > instance.getBudget();
        });
        const ptrdiff_t new_size = std::distance(zip_view.begin(), end);
        sorted_items.erase(sorted_items.begin() + new_size, sorted_items.end());
        permuted_id.erase(permuted_id.begin() + new_size, permuted_id.end());
        ranges::sort(zip_view,
                     [](auto p1, auto p2) { return p1.first < p2.first; });

        const int nb_threads = std::thread::hardware_concurrency();

        best_value = 0;
        available_threads = nb_threads - 1;
        threads.emplace(iterative_bnb, Node(this));

        while(available_threads < nb_threads) {
            threads.p
        }

        TSolution solution(instance);
        while(!best_stack.empty()) {
            solution.add(permuted_id[best_stack.top()]);
            best_stack.pop();
        }
        return solution;
    }
};
}  // namespace Knapstack

#endif  // PARALLEL_BRANCH_AND_BOUND_HPP