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
    private:
        int _id;
    public:
        Item(int v, int c, int i) : value{v}, cost{c}, _id{i} {}
        Item(const Item & item) : value{item.value}, cost{item.cost}, _id{item._id} {}
        double getRatio() const { 
            if(cost == 0) return std::numeric_limits<double>::max();
            return value / (double)cost;
        }
        void setId(int i) { _id = i; } 
        int id() { return _id; } 
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

        void addItem(int v, int w) { items.push_back(Item(v, w, items.size())); }
        void clearItems() { items.clear(); }
        int itemCount() const { return items.size(); }

        const Item get(int i) const { return items[i]; }
        const Item operator[](int i) const { return get(i); }

        void sortByRatios() {
            std::sort(items.begin(), items.end());
            for(int i=0; i<static_cast<int>(items.size()); ++i) {
                items[i].setId(i);
            }
        }
};

class Solution {
    private:
        const Instance & instance;
        std::vector<bool> taken;
        int total_value;
        int total_cost;
    public:
        Solution(const Instance & i) : instance(i), taken(i.itemCount()), total_value{0}, total_cost{0} {}

        int getValue() { return total_value; }
        int getCost() { return total_cost; }

        void take(int i) { 
            assert(!taken[i]);
            taken[i] = true;
            const Item item = instance[i];
            total_value += item.value;
            total_cost += item.cost;
        }
        void release(int i) { 
            assert(taken[i]);
            taken[i] = false;
            const Item item = instance[i];
            total_value -= item.value;
            total_cost -= item.cost;
        }

        double computeUpperBound(int from_depth) {
            int current_value = total_value;
            int budget_left = instance.getBudget() - total_cost;
            for(int i=from_depth; i<instance.itemCount(); ++i) {
                assert(taken[i] == false);
                const Item item = instance[i];
                if(budget_left - item.cost <= 0)
                    return current_value + (budget_left / (double)item.cost) * item.value;
                budget_left -= item.cost;
                current_value += item.value;
            }
            return current_value;
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

void branch_and_bound(const Instance & instance, Solution & solution, int depth, int & best_known) {
    if(solution.getCost() > instance.getBudget()) return; // invalid node
    if(depth == instance.itemCount()) { // leaf
        if(best_known < solution.getValue())
            best_known = solution.getValue();        
        return;
    }
    if(solution.computeUpperBound(depth) < best_known) return; // this node could not be in a better solution

    solution.take(depth);
    branch_and_bound(instance, solution, depth+1, best_known);
    solution.release(depth);
    branch_and_bound(instance, solution, depth+1, best_known);
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

    int best = 0;
    Solution solution(instance);
    branch_and_bound(instance, solution, 0, best);

    std::cout << best << std::endl;

    return EXIT_SUCCESS;
}

