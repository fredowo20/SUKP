#include <bits/stdc++.h>
using namespace std;

struct Item {
    int weight;
    int profit;
    vector<int> elements;
};

bool operator<(const Item& a, const Item& b) {
    return a.weight < b.weight;
}

struct Element {
    int weight;
    int num_items;
    vector<int> items;
};

bool operator<(const Element& a, const Element& b) {
    return a.weight < b.weight;
}

int knapsackByElements(vector<int>& elements, vector<vector<int>>& items, vector<int>& profits, int maxWeight) {
    vector<Item> itemList;
    vector<Element> elementList;

    // Initialize item list and element list
    for (int i = 0; i < items.size(); i++) {
        int weight = 0;
        for (int j = 0; j < elements.size(); j++) {
            if (items[i][j] == 1) {
                weight += elements[j];
            }
        }
        itemList.push_back({weight, profits[i], items[i]});
    }
    for (int i = 0; i < elements.size(); i++) {
        int num_items = 0;
        vector<int> items_list;
        for (int j = 0; j < items.size(); j++) {
            if (items[j][i] == 1) {
                num_items++;
                items_list.push_back(j);
            }
        }
        elementList.push_back({elements[i], num_items, items_list});
    }

    // Sort element list by weight
    sort(elementList.begin(), elementList.end());

    // Initialize sets for items and elements
    set<int> chosenItems;
    set<int> chosenElements;

    int maxProfit = 0;

    // Choose elements
    for (int i = 0; i < elementList.size(); i++) {
        int elementWeight = elementList[i].weight;
        int numItems = elementList[i].num_items;
        vector<int> itemsList = elementList[i].items;

        // Sort items by profit
        vector<Item> sortedItems;
        for (int j = 0; j < itemsList.size(); j++) {
            sortedItems.push_back(itemList[itemsList[j]]);
        }
        sort(sortedItems.rbegin(), sortedItems.rend());

        int itemsCompleted = 0;
        for (int j = 0; j < sortedItems.size(); j++) {
            if (chosenItems.count(itemsList[j]) > 0) {
                itemsCompleted++;
                continue;
            }
            if (elementWeight > maxWeight) {
                break;
            }
            if (itemsCompleted == numItems) {
                break;
            }
            if (elementWeight + sortedItems[j].weight <= maxWeight) {
                chosenItems.insert(itemsList[j]);
                chosenElements.insert(i);
                elementWeight += sortedItems[j].weight;
                maxProfit += sortedItems[j].profit;
                itemsCompleted++;
            }
        }
    }

    return maxProfit;
}

int main() {
    vector<int> elements = {10, 20, 30};
    vector<vector<int>> items = {{1, 1, 0}, {1, 0, 1}, {0, 1, 1}};
    vector<int> profits = {60, 100, 120};
    int maxWeight = 50;
    cout << knapsackByElements(elements, items, profits, maxWeight) << endl;
    return 0;
}
