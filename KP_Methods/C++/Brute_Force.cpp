#include <iostream>
#include <vector>
using namespace std;

int get_weight(vector<int> item, vector<int> elements) {
    int weight = 0;
    for (int i = 0; i < elements.size(); i++) {
        if (item[i] == 1) {
            weight += elements[i];
        }
    }
    return weight;
}

vector<vector<int>> subsets(vector<vector<int>> items) {
    vector<vector<int>> subsets = {{}};
    for (vector<int> item : items) {
        int n = subsets.size();
        for (int i = 0; i < n; i++) {
            vector<int> new_subset;
            for (int j = 0; j < subsets[i].size(); j++) {
                new_subset.push_back(subsets[i][j]);
            }
            for (int j = 0; j < item.size(); j++) {
                new_subset.push_back(item[j]);
            }
            subsets.push_back(new_subset);
        }
    }
    return subsets;
}

int knapsack_bruteForce(vector<int> elements, vector<vector<int>> items, vector<int> profits, int max_weight) {
    // Helper function to generate all possible subsets of items
    vector<vector<int>> all_subsets = subsets(items);

    // Initialize the maximum profit and the best subset of items
    int max_profit = 0;
    vector<int> best_subset;

    for (vector<int> subset : all_subsets) {
        if (get_weight(subset, elements) <= max_weight) {
            int subset_profit = 0;
            for (int i = 0; i < subset.size(); i++) {
                if (subset[i] == 1) {
                    subset_profit += profits[i];
                }
            }
            if (subset_profit > max_profit) {
                max_profit = subset_profit;
                best_subset = subset;
            }
        }
    }
    return max_profit;
}

int main() {
    vector<int> elements = {10, 20, 30};
    vector<vector<int>> items = {{1, 1, 0}, {1, 0, 1}, {0, 1, 1}};
    vector<int> profits = {60, 100, 120};
    int max_weight = 50;
    cout << knapsack_bruteForce(elements, items, profits, max_weight) << endl;
    return 0;
}
