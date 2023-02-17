def knapsack_bruteForce(elements, items, profits, max_weight):
    # Helper function to calculate the weight of an item
    def get_weight(item):
        weight = 0
        for i in range(len(item)):
            if item[i] == 1:
                weight += elements[i]
        return weight

    # Helper function to generate all possible subsets of items
    def subsets(items):
        subsets = [[]]
        for item in items:
            for i in range(len(subsets)):
                subsets.append(subsets[i] + [item])
        return subsets

    # Initialize the maximum profit and the best subset of items
    max_profit = 0
    best_subset = []
    all_subsets = subsets(range(len(items)))
    for subset in all_subsets:
        subset_items = [items[i] for i in subset]
        if get_weight(subset_items) <= max_weight:
            subset_profit = sum([profits[i] for i in subset])
            if subset_profit > max_profit:
                max_profit = subset_profit
                best_subset = subset_items
    return max_profit

# Example usage
elements = [10, 20, 30]
items = [[1, 1, 0], [1, 0, 1], [0, 1, 1]]
profits = [60, 100, 120]
max_weight = 50
print(knapsack_bruteForce(elements, items, profits, max_weight))

import pandas as pd
pd.options.mode.chained_assignment = None  # default='warn'
import numpy as np

# Selection of instance
data = pd.read_csv(r'C:\Users\Javier\Documents\IA\Fondecyt\KP_Methods\Instances\InstancesofSetI\85_100_0.10_0.75.txt', header=None)

n_items = data[0][0]
n_elements = data[0][1]
max_weight = int(data[0][2])

profits = str(data[0][3]).split()
for i in range(len(profits)):
    profits[i] = int(profits[i])

elements = str(data[0][4]).split()
for i in range(len(elements)):
    elements[i] = int(elements[i])

items = data.tail(-5)
items.reset_index(inplace = True, drop = True)

for i in range(len(items)):
    items[0][i] = str(items[0][i]).split()
for i in range(len(items)):
    for j in range(len(elements)):
        items[0][i][j] = int(items[0][i][j])

items = items[0].values.tolist()

print(knapsack_bruteForce(elements, items, profits, max_weight))
