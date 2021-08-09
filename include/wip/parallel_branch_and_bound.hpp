#ifndef PARALLEL_BRANCH_AND_BOUND_HPP
#define PARALLEL_BRANCH_AND_BOUND_HPP

#include <atomic>
#include <mutex>
#include <numeric>
#include <thread>
#include <vector>

#include <tbb/concurrent_queue.h>

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


        std::atomic<int> available_threads;
        std::atomic<bool> thread_loop;

        std::mutex solution_mutex;
        std::stack<int> best_stack;
        std::atomic<Value> best_value;
        
        struct Node;
        tbb::concurrent_bounded_queue<Node> node_queue;

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

            Node& operator=(const Node & n) = default;
            Node& operator=(Node && n) = default;

            double computeUpperBound(size_t depth, Value bound_value, Cost bound_budget_left) { 
                for(; depth<solver->sorted_items.size(); ++depth) {
                    const TItem & item = solver->sorted_items[depth];
                    if(bound_budget_left <= item.cost)
                        return bound_value + bound_budget_left * item.getRatio();
                    bound_budget_left -= item.cost;
                    bound_value += item.value;
                }
                return bound_value;
            }
            
            void iterative_bnb() {
                goto begin;
            backtrack:
                while(!stack.empty()) {
                    depth = stack.top();
                    stack.pop();
                    value -= solver->sorted_items[depth].value;
                    budget_left += solver->sorted_items[depth++].cost;
                    for(; depth<solver->sorted_items.size(); ++depth) {
                        if(budget_left < solver->sorted_items[depth].cost) continue;
                        if(computeUpperBound(depth, value, budget_left) <= solver->best_value)
                            goto backtrack;
                    begin:
                        // if(solver->sorted_items.size() - depth > 1000) {
                        if(--solver->available_threads > 0) {
                            solver->node_queue.emplace(*this);
                            continue;
                        } else {
                            ++solver->available_threads;
                        }
                        // }
                        value += solver->sorted_items[depth].value;
                        budget_left -= solver->sorted_items[depth].cost;
                        stack.push(depth);
                    }
                    if(value <= solver->best_value) 
                        continue;
                    solver->best_value = value;
                    solver->solution_mutex.lock();
                    solver->best_stack = stack;
                    solver->solution_mutex.unlock();
                }
            }
        };
    public:
        ParallelBranchAndBound(TInstance & instance)
            : instance(instance) {}
    
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
            ranges::sort(zip_view, [](auto p1, auto p2){ return p1.first < p2.first; });
            

            const int nb_threads = std::thread::hardware_concurrency();
            available_threads = nb_threads-1;
            thread_loop = true;

            node_queue.clear();
            node_queue.set_capacity(nb_threads);
            node_queue.emplace(this);

            best_value = 0;
            std::vector<std::thread> threads;
            threads.reserve(nb_threads);
            for(int i=0; i<nb_threads; ++i) {
                threads.emplace_back([this, &nb_threads] (void) {
                    Node n;
                    for(;;) {
                        while(!node_queue.try_pop(n))
                            if(!thread_loop) return;
                        n.iterative_bnb();
                        ++available_threads;
                        if(available_threads == nb_threads) {
                            thread_loop = false;
                            return;
                        }
                    }
                });
            }
            for(auto & thread : threads)
                thread.join();

            TSolution solution(instance);
            while(! best_stack.empty()) {
                solution.add(permuted_id[best_stack.top()]);
                best_stack.pop();
            }
            return solution;
        }
    };
} //namespace Knapstack

#endif //PARALLEL_BRANCH_AND_BOUND_HPP