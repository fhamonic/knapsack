#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <vector>

#include <atomic>
#include <thread>
#include <mutex>
#include <numeric>

#include <range/v3/all.hpp>

namespace Knapstack {
    template <typename Value, typename Cost>
    class Instance {
    public:
        class Item {
        public:
            Value value;
            Cost cost;
        public:
            Item(Value v, Cost c) : value{v}, cost{c} {}
            Item(const Item & item) : value{item.value}, cost{item.cost} {}
            double getRatio() const { 
                if(cost == 0) return std::numeric_limits<double>::max();
                return value / (double)cost;
            }
            bool operator<(const Item& other) const { return getRatio() > other.getRatio(); }
        };
    private:
        Cost budget;
        std::vector<Item> items;
    public:
        Instance() {}

        void setBudget(Cost b) { budget = b; }
        Cost getBudget() const { return budget; }

        void addItem(Value v, Cost w) { items.push_back(Item(v, w)); }
        size_t itemCount() const { return items.size(); }

        const std::vector<Item> & getItems() const { return items; }
        const Item getItem(int i) const { return items[i]; }
        const Item operator[](int i) const { return getItem(i); }
    };

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

    template <template<typename,typename> class Inst, typename Value, typename Cost>
    class BranchAndBound {
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
            std::vector<bool> takens;
            Value value;
            Cost budget_left;
            size_t depth;

            Node(const TInstance & instance)
                    : takens(instance.itemCount(), false)
                    , value{0}
                    , budget_left{instance.getBudget()}
                    , depth{0} {}
        };
    public:
        BranchAndBound(TInstance & instance)
            : instance(instance)
            , best_takens(instance.itemCount(), false) {}
    
        double computeUpperBound(const Node & n) {
            int value = n.value;
            int budget_left = n.budget_left;            
            for(size_t i=n.depth; i<instance.itemCount(); ++i) {
                const TItem item = sorted_items[i];
                if(budget_left <= item.cost)
                    return (double)value + (double)budget_left * item.getRatio();
                budget_left -= item.cost;
                value += item.value;
            }
            return value;
        }

        void recursive_bnb(Node & n) {
            if(n.depth == instance.itemCount()) { // leaf
                if(n.value > best_value) {
                    best_value = n.value;
                    solution_mutex.lock();
                    best_takens = n.takens;
                    solution_mutex.unlock();
                }
                return;
            }
            if(computeUpperBound(n) <= best_value)
                return; // this node could not be in a better solution
            const TItem & item = sorted_items[n.depth];
            if(item.cost <= n.budget_left) {
                n.takens[n.depth] = true;
                n.value += item.value;
                n.budget_left -= item.cost;
                ++n.depth;
                recursive_bnb(n);
                --n.depth;
                n.takens[n.depth] = false;
                n.value -= item.value;
                n.budget_left += item.cost;
            }
            ++n.depth;
            recursive_bnb(n);
            --n.depth;
        }

        void thread_bnb(Node n) {
            recursive_bnb(n);
        }

        TSolution solve() {
            sorted_items = instance.getItems();
            std::vector<int> permuted_id(instance.itemCount());
            std::iota(permuted_id.begin(), permuted_id.end(), 0);

            auto zip_view = ranges::view::zip(sorted_items, permuted_id);
            ranges::sort(zip_view, [](auto p1, auto p2){ return p1.first < p2.first; });
            
            best_value = 0;
            Node n(instance);
            recursive_bnb(n);

            TSolution solution(instance);
            for(size_t i=0; i<instance.itemCount(); ++i) {
                if(best_takens[i])
                    solution.add(permuted_id[i]);
            }

            std::cout << best_value << std::endl;

            return solution;
        }
    };
}; //namespace Knapstack

Knapstack::Instance<int,int> parse_instance(const std::filesystem::path & instance_path) {
    Knapstack::Instance<int,int> instance;
    std::ifstream file(instance_path);
    int budget;
    file >> budget;
    instance.setBudget(budget);
    int value, weight;
    while(file >> weight >> value) instance.addItem(value, weight);
    return instance;
}

int main(int argc, const char *argv[]) {
    if(argc < 2) {
        std::cerr << "input requiered : <knapstack_instance_file>" << std::endl;
        return EXIT_FAILURE;
    }
    std::filesystem::path instance_path = argv[1];
    if(!std::filesystem::exists(instance_path)) {
        std::cerr << instance_path << ":" << " File does not exists" << std::endl;
        return EXIT_FAILURE;
    }
    
    Knapstack::Instance instance = parse_instance(instance_path);
    Knapstack::BranchAndBound solver(instance);
    Knapstack::Solution solution = solver.solve();

    std::cout << solution.getValue() << std::endl;

    return EXIT_SUCCESS;
}

