def knapsack_by_elements(elements, items, profits, max_weight):
    # Calculate the weights of the elements and the number of elements in each item
    element_weights = [0] * len(elements)
    element_count = [0] * len(elements)
    for i, item in enumerate(items):
        for j, elem in enumerate(item):
            if elem == 1:
                element_weights[j] += elements[j]
                element_count[j] += 1

    # Sort the elements by their weight in descending order
    sorted_elements = sorted(range(len(elements)), key=lambda k: -element_weights[k])

    # Initialize the knapsack and the total profit
    knapsack = [0] * len(items)
    total_profit = 0

    # Iterate through the sorted elements and add them to the knapsack
    for elem in sorted_elements:
        # Skip elements that are already in the knapsack or that are too heavy
        if element_count[elem] == 0 or elements[elem] > max_weight:
            continue

        # Find the items that contain this element and sort them by their remaining weight
        eligible_items = [(i, sum([elements[k] for k, e in enumerate(items[i]) if e == 1])) for i in range(len(items)) if items[i][elem] == 1]
        eligible_items.sort(key=lambda x: x[1])

        # Iterate through the eligible items and add them to the knapsack
        for item in eligible_items:
            item_idx = item[0]
            item_weight = item[1]
            if item_weight <= max_weight:
                # Add the item to the knapsack
                knapsack[item_idx] = 1
                element_count[elem] -= 1
                max_weight -= item_weight
                total_profit += profits[item_idx]
                break

    return total_profit

# Example usage
elements = [10, 20, 30]
items = [[1, 1, 0], [1, 0, 1], [0, 1, 1]]
profits = [60, 100, 120]
max_weight = 50
print(knapsack_by_elements(elements, items, profits, max_weight))