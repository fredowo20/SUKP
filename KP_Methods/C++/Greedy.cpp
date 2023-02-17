#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Helper function to calculate the weight of an item
int get_weight(vector<int> item, vector<int> elements) {
    int weight = 0;
    for (int i = 0; i < item.size(); i++) {
        if (item[i] == 1) {
            weight += elements[i];
        }
    }
    return weight;
}

// Helper function to calculate the value/weight ratio of an item
double get_value_per_weight(int item, vector<int> profits, vector<vector<int>> items, vector<int> elements) {
    return (double) profits[item] / get_weight(items[item], elements);
}

int knapsack_greedy(vector<int> elements, vector<vector<int>> items, vector<int> profits, int max_weight) {
    // Initialize an empty knapsack
    int knapsack_weight = 0;
    int knapsack_profit = 0;

    // Sort items by value/weight ratio
    vector<int> item_list(items.size());
    for (int i = 0; i < items.size(); i++) {
        item_list[i] = i;
    }
    sort(item_list.begin(), item_list.end(), [&](int a, int b) {
        return get_value_per_weight(a, profits, items, elements) > get_value_per_weight(b, profits, items, elements);
    });

    // Iterate through items and add them to the knapsack if they fit
    for (int i = 0; i < item_list.size(); i++) {
        int item = item_list[i];
        if (knapsack_weight + get_weight(items[item], elements) <= max_weight) {
            knapsack_weight += get_weight(items[item], elements);
            knapsack_profit += profits[item];
        }
    }

    // Return the items in the knapsack and the total profit
    return knapsack_profit;
}

int main() {
    vector<int> elements = {10, 20, 30};
    vector<vector<int>> items = {{1, 1, 0}, {1, 0, 1}, {0, 1, 1}};
    vector<int> profits = {60, 100, 120};
    int max_weight = 50;

    cout << knapsack_greedy(elements, items, profits, max_weight) << endl;

    return 0;
}
