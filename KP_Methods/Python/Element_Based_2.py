def knapsack_by_elements2(elements, items, profits, max_weight):
    n = len(elements)

    # Create a list of tuples for each element, containing its index and its weight
    element_list = [(i, elements[i]) for i in range(n)]

    # Sort the element list in decreasing order of weight
    element_list.sort(key=lambda x: -x[1])

    # Create a dictionary to store the number of remaining items that contain each element
    remaining_items = {}
    for i in range(n):
        remaining_items[i] = len([j for j in range(len(items)) if items[j][i] == 1])

    # Initialize the current weight and profit to zero, and the completed items to an empty list
    current_weight = 0
    current_profit = 0
    completed_items = []

    # Loop over the elements in the element list
    for element in element_list:
        element_idx = element[0]
        element_weight = element[1]

        # If the element has no remaining items, skip it
        if remaining_items[element_idx] == 0:
            continue

        # Create a list of items that contain the current element
        candidate_items = [i for i in range(len(items)) if items[i][element_idx] == 1]

        # Sort the candidate items in decreasing order of the number of remaining elements needed
        candidate_items.sort(key=lambda x: sum([1 for j in range(n) if items[x][j] == 1 and remaining_items[j] > 0]))

        # Loop over the candidate items
        for item_idx in candidate_items:
            # If the item has already been completed, skip it
            if item_idx in completed_items:
                continue

            # Calculate the weight and profit of the item
            item_weight = sum([elements[i] for i in range(n) if items[item_idx][i] == 1])
            item_profit = profits[item_idx]

            # If adding the item would exceed the maximum weight, skip it
            if current_weight + item_weight > max_weight:
                continue

            # Add the item to the knapsack and update the current weight and profit
            completed_items.append(item_idx)
            current_weight += item_weight
            current_profit += item_profit

            # Decrement the remaining items count for each element in the item
            for i in range(n):
                if items[item_idx][i] == 1:
                    remaining_items[i] -= 1

            # Break out of the loop over candidate items
            break

    # Return the final profit
    return current_profit

# Example usage
elements = [10, 20, 30]
items = [[1, 1, 0], [1, 0, 1], [0, 1, 1]]
profits = [60, 100, 120]
max_weight = 50
print(knapsack_by_elements2(elements, items, profits, max_weight))

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

print(knapsack_by_elements2(elements, items, profits, max_weight))
