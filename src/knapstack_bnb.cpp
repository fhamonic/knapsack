#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <vector>

#include <atomic>
#include <thread>
#include <mutex>

namespace Knapstack {
    class Item {
    public:
        int value;
        int cost;
    public:
        Item(int v, int c) : value{v}, cost{c} {}
        Item(const Item & item) : value{item.value}, cost{item.cost} {}
        double getRatio() const { 
            if(cost == 0) return std::numeric_limits<double>::max();
            return value / (double)cost;
        }
        bool operator<(const Item& other) const { return getRatio() > other.getRatio(); }
    };

    class Instance {
    private:
        int budget;
        std::vector<Item> items;
    public:
        Instance() {}

        void setBudget(int b) { budget = b; }
        int getBudget() const { return budget; }

        void addItem(int v, int w) { items.push_back(Item(v, w)); }
        int itemCount() const { return items.size(); }

        const std::vector<Item> & getItems() const { return items; }
        const Item getItem(int i) const { return items[i]; }
        const Item operator[](int i) const { return getItem(i); }
    };

    class Solution {
    private:
        const Instance & instance;
        std::vector<bool> _taken;
    public:
        Solution(const Instance & i) : instance(i), _taken(i.itemCount()) {}

        void add(int i) { _taken[i] = true; }
        void remove(int i) { _taken[i] = false; }
        bool isTaken(int i) { return _taken[i]; }

        int getValue() const {
            int sum = 0;
            for(int i=0; i<instance.itemCount(); ++i)
                sum += _taken[i] ? instance[i].value : 0;
            return sum;
        }
        int getCost() const { 
            int sum = 0;
            for(int i=0; i<instance.itemCount(); ++i)
                sum += _taken[i] ? instance[i].cost : 0;
            return sum;
        }
    };

    class BranchAndBound {
    private:
        const Instance & instance;
        std::vector<Item> sorted_items;

        std::mutex solution_mutex;
        Solution * best_solution;
        std::atomic<double> best_value;
        
    public:
        BranchAndBound(Instance & instance)
            : instance(instance) {}
    
        double computeUpperBound(int from_depth, int value, int budget_left) {
            for(int i=from_depth; i<instance.itemCount(); ++i) {
                const Item item = sorted_items[i];
                if(budget_left - item.cost <= 0)
                    return (double)value + (double)budget_left * item.getRatio();
                budget_left -= item.cost;
                value += item.value;
            }
            return value;
        }

        void recursive_branch_and_bound(int depth, int value, int budget_left) {
            if(budget_left < 0) return; // invalid node
            if(depth == instance.itemCount()) { // leaf
                if(value > best_value) {
                    solution_mutex.lock();
                    // best_solution = current_solution;
                    solution_mutex.unlock();
                }
                return;
            }
            if(computeUpperBound(depth, value, budget_left) <= best_value)
                return; // this node could not be in a better solution
            const Item & item = instance[depth];
            if(item.cost <= budget_left)
                recursive_branch_and_bound(depth+1, value+item.value, budget_left-item.cost);
            recursive_branch_and_bound(depth+1, value, budget_left);
        }

        Solution solve() {
            sorted_items = instance.getItems();
            std::sort(sorted_items.begin(), sorted_items.end());
            Solution solution(instance);
            best_solution = &solution;
            recursive_branch_and_bound(0, 0, 0);
            return solution;
        }
    };
}; //namespace Knapstack

Knapstack::Instance parse_instance(const std::filesystem::path & instance_path) {
    Knapstack::Instance instance;
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

