def knapsack_greedy(elements, items, profits, max_weight):
    # Helper function to calculate the weight of an item
    def get_weight(item):
        weight = 0
        for i in range(len(item)):
            if item[i] == 1:
                weight += elements[i]
        return weight

    # Helper function to calculate the value/weight ratio of an item
    def get_value_per_weight(item):
        return profits[item] / get_weight(items[item])

    # Initialize an empty knapsack
    knapsack_weight = 0
    knapsack_profit = 0

    # Sort items by value/weight ratio
    item_list = list(range(len(items)))
    item_list.sort(key=get_value_per_weight, reverse=True)

    # Iterate through items and add them to the knapsack if they fit
    for item in item_list:
        if knapsack_weight + get_weight(items[item]) <= max_weight:
            knapsack_weight += get_weight(items[item])
            knapsack_profit += profits[item]

    # Return the items in the knapsack and the total profit
    return knapsack_profit

# Example usage
elements = [10, 20, 30]
items = [[1, 1, 0], [1, 0, 1], [0, 1, 1]]
profits = [60, 100, 120]
max_weight = 50
print(knapsack_greedy(elements, items, profits, max_weight))

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

print(knapsack_greedy(elements, items, profits, max_weight))
