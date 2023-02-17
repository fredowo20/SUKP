def knapsack_dynamic(elements, items, profits, max_weight):
    # Initialize a 2D array with all -1s
    # Rows represent items, columns represent weight
    dp = [[-1 for _ in range(max_weight + 1)] for _ in range(len(items) + 1)]
    
    # Helper function to calculate the weight of an item
    def get_weight(item):
        weight = 0
        for i in range(len(item)):
            if item[i] == 1:
                weight += elements[i]
        return weight

    # Helper function to calculate the maximum value for a given weight and set of items
    def knapsack_helper(items, max_weight, idx, dp):
        # If the weight is 0 or there are no more items, return 0
        if max_weight == 0 or idx == 0:
            return 0

        # If the result is already calculated, return it
        if dp[idx][max_weight] != -1:
            return dp[idx][max_weight]

        # If the weight of the current item is greater than the current weight,
        # skip the current item
        if get_weight(items[idx-1]) > max_weight:
            dp[idx][max_weight] = knapsack_helper(items, max_weight, idx-1, dp)
            return dp[idx][max_weight]

        # The maximum value is the maximum of either the value without the current item,
        # or the value with the current item plus the value of the remaining weight
        dp[idx][max_weight] = max(knapsack_helper(items, max_weight, idx-1, dp), profits[idx-1] + knapsack_helper(items, max_weight - get_weight(items[idx-1]), idx-1, dp))
        return dp[idx][max_weight]

    # Return the maximum value that can be achieved with the given weight constraint
    return knapsack_helper(items, max_weight, len(items), dp)

# Example usage
elements = [10, 20, 30]
items = [[1, 1, 0], [1, 0, 1], [0, 1, 1]]
profits = [60, 100, 120]
max_weight = 50

print(knapsack_dynamic(elements, items, profits, max_weight))

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

print(knapsack_dynamic(elements, items, profits, max_weight))
