#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <vector>

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
        bool operator<(const Item& other) const { 
            if(getRatio() == other.getRatio())
                return cost > other.cost;
            return getRatio() > other.getRatio();
        }
};

class Instance {
    private:
        int budget;
        std::vector<Item> items;
    public:
        std::vector<int> min_cost_below;
        Instance() {}

        void setBudget(int b) { budget = b; }
        int getBudget() const { return budget; }

        void addItem(int v, int w) { items.push_back(Item(v, w)); }
        void clearItems() { items.clear(); }
        int itemCount() const { return items.size(); }

        const Item getItem(int i) const { return items[i]; }
        const Item operator[](int i) const { return getItem(i); }

        void sortByRatios() { 
            std::sort(items.begin(), items.end()); // implicitly uses operator< of Item
            min_cost_below.resize(itemCount());
            int i = itemCount()-1;
            min_cost_below[i] = items[i].cost;
            for(--i; i>=0; --i)
                min_cost_below[i] = std::min(min_cost_below[i+1], items[i].cost);
        } 
};

class Solution {
    private:
        const Instance & instance;
        std::vector<bool> _taken;
        int total_value;
        int total_cost;
    public:
        Solution(const Instance & i) : instance(i), _taken(i.itemCount()), total_value{0}, total_cost{0} {}

        int getValue() { return total_value; }
        int getCost() { return total_cost; }

        void take(int i) { 
            assert(!_taken[i]);
            _taken[i] = true;
            const Item item = instance[i];
            total_value += item.value;
            total_cost += item.cost;
        }
        void release(int i) { 
            assert(_taken[i]);
            _taken[i] = false;
            const Item item = instance[i];
            total_value -= item.value;
            total_cost -= item.cost;
        }

        bool isTaken(int i) { return _taken[i]; }

        bool canProgress(int from_depth) {
            int budget_left = instance.getBudget() - total_cost;
            return instance.min_cost_below[from_depth] <= budget_left;
        }
        double computeUpperBound(int from_depth) {
            int current_value = total_value;
            int budget_left = instance.getBudget() - total_cost;
            for(int i=from_depth; i<instance.itemCount(); ++i) {
                assert(!_taken[i]);
                const Item item = instance[i];
                if(budget_left - item.cost <= 0)
                    return current_value + budget_left * item.getRatio();
                budget_left -= item.cost;
                current_value += item.value;
            }
            return current_value;
        }

        void operator=(const Solution & other) {
            assert(instance.itemCount() == other.instance.itemCount());
            for(int i=0; i<instance.itemCount(); ++i)
                _taken[i] = other._taken[i];
            total_value = other.total_value;
            total_cost = other.total_cost;
        }
};

void fill_instance(const std::filesystem::path & instance_path, Instance & instance) {
    std::ifstream file(instance_path);
    int budget;
    file >> budget;
    instance.setBudget(budget);
    int value, weight;
    while(file >> weight >> value) instance.addItem(value, weight);
}

void branch_and_bound(const Instance & instance, Solution & current_solution, int depth, Solution & best_solution) {
    if(current_solution.getCost() > instance.getBudget()) return; // invalid node
    if(depth == instance.itemCount() || !current_solution.canProgress(depth)) { // leaf or not enought budget
        if(current_solution.getValue() > best_solution.getValue())
            best_solution = current_solution;
        // std::cout << current_solution.getValue() << "\t" << instance.getBudget() - current_solution.getCost() << "\t" << best_solution.getValue() << std::endl;
        return;
    }
    if(current_solution.computeUpperBound(depth) <= best_solution.getValue()) return; // this node could not be in a better solution

    // if(depth > 100)
    //     std::cout << depth << "\t" << current_solution.computeUpperBound(depth) << std::endl; 

    current_solution.take(depth);
    branch_and_bound(instance, current_solution, depth+1, best_solution);
    current_solution.release(depth);
    branch_and_bound(instance, current_solution, depth+1, best_solution);
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
    
    Instance instance;
    fill_instance(instance_path, instance);
    instance.sortByRatios();

    Solution current_solution(instance), best_solution(instance);
    branch_and_bound(instance, current_solution, 0, best_solution);

    std::cout << best_solution.getValue() << std::endl;

    return EXIT_SUCCESS;
}

