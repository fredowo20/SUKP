#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

int knapsack_by_elements(vector<int>& elements, vector<vector<int>>& items, vector<int>& profits, int max_weight) {
    int n = elements.size();
    int m = items.size();

    // Calculate the weight of each item and store it in a map
    unordered_map<int, int> item_weights;
    for (int i = 0; i < m; i++) {
        int weight = 0;
        for (int j = 0; j < n; j++) {
            if (items[i][j] == 1) {
                weight += elements[j];
            }
        }
        item_weights[i] = weight;
    }

    // Sort the items in decreasing order of weight and store their indices in a vector
    vector<int> item_indices(m);
    for (int i = 0; i < m; i++) {
        item_indices[i] = i;
    }
    sort(item_indices.begin(), item_indices.end(), [&](int i, int j) {
        return item_weights[i] > item_weights[j];
    });

    // Initialize a map to keep track of the remaining elements for each item
    unordered_map<int, vector<int>> remaining_elements;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (items[i][j] == 1) {
                remaining_elements[i].push_back(j);
            }
        }
    }

    int total_profit = 0;

    // Iterate through the items in decreasing order of weight
    for (int i : item_indices) {
        // If the weight of the current item is greater than the remaining capacity,
        // skip the item
        if (item_weights[i] > max_weight) {
            continue;
        }

        // Iterate through the remaining elements of the current item
        for (int j = 0; j < remaining_elements[i].size(); j++) {
            int element = remaining_elements[i][j];

            // If the weight of the current element is greater than the remaining capacity,
            // skip the element
            if (elements[element] > max_weight) {
                continue;
            }

            // Add the profit of the current item to the total profit
            total_profit += profits[i];

            // Update the remaining capacity
            max_weight -= elements[element];

            // Remove the current element from the remaining elements of all items
            for (auto& kv : remaining_elements) {
                auto& elements = kv.second;
                elements.erase(remove(elements.begin(), elements.end(), element), elements.end());
            }

            // Remove the current item from the list of remaining items
            item_indices.erase(remove(item_indices.begin(), item_indices.end(), i), item_indices.end());

            // Decrement the index and break out of the loop
            j--;
            break;
        }
    }

    return total_profit;
}

int main() {
    vector<int> elements = {10, 20, 30};
    vector<vector<int>> items = {{1, 1, 0}, {1, 0, 1}, {0, 1, 1}};
    vector<int> profits = {60, 100, 120};
    int max_weight = 50;

    cout << knapsack_by_elements(elements, items, profits, max_weight) << endl;

    return 0;
}
