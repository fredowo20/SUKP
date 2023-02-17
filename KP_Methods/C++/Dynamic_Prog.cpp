#include <iostream>
#include <vector>
#include <cstring>
using namespace std;

int get_weight(vector<int> item, vector<int> elements) {
    int weight = 0;
    for (int i = 0; i < item.size(); i++) {
        if (item[i] == 1) {
            weight += elements[i];
        }
    }
    return weight;
}

int knapsack_helper(vector<vector<int>>& items, vector<int>& profits, vector<int>& elements, int max_weight, int idx, vector<vector<int>>& dp) {
    if (max_weight == 0 || idx == 0) {
        return 0;
    }
    if (dp[idx][max_weight] != -1) {
        return dp[idx][max_weight];
    }
    if (get_weight(items[idx-1], elements) > max_weight) {
        dp[idx][max_weight] = knapsack_helper(items, profits, elements, max_weight, idx-1, dp);
        return dp[idx][max_weight];
    }
    dp[idx][max_weight] = max(knapsack_helper(items, profits, elements, max_weight, idx-1, dp), profits[idx-1] + knapsack_helper(items, profits, elements, max_weight - get_weight(items[idx-1], elements), idx-1, dp));
    return dp[idx][max_weight];
}

int knapsack_dynamic(vector<int>& elements, vector<vector<int>>& items, vector<int>& profits, int max_weight) {
    int n = items.size();
    vector<vector<int>> dp(n+1, vector<int>(max_weight+1, -1));
    return knapsack_helper(items, profits, elements, max_weight, n, dp);
}

int main() {
    vector<int> elements = {10, 20, 30};
    vector<vector<int>> items = {{1, 1, 0}, {1, 0, 1}, {0, 1, 1}};
    vector<int> profits = {60, 100, 120};
    int max_weight = 50;
    cout << knapsack_dynamic(elements, items, profits, max_weight) << endl;
    return 0;
}
