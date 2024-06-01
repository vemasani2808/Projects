import math

# Define a class to represent edge data
class Edge:
    def __init__(self, source_node, target_node, timestamps_and_rates):
        self.source_node = source_node
        self.target_node = target_node
        self.timestamps_and_rates = timestamps_and_rates

# Define a function to read edge data from a file
def read_edge_data(filename):
    with open(filename, 'r') as file:
        line = file.readline().strip()
        source_node, target_node, *data = line.split(',')
        timestamps_and_rates = {float(data[i]): float(data[i+1]) for i in range(0, len(data), 2)}
        return Edge(int(source_node), int(target_node), timestamps_and_rates)

# Define a function to compute performance metrics
def compute_performance(inferred_edge, true_edge, MinAlpha):
    accuracies = []

    # Iterate over time steps
    for t in range(10, 110, 10):
        CurrentAccuracyDenominator = 0.0
        CurrentPrecision = 0.0

        inferred_rate = inferred_edge.timestamps_and_rates.get(float(t), 0.0)
        true_rate = true_edge.timestamps_and_rates.get(float(t), 0.0)

        # Update accuracy denominator
        CurrentAccuracyDenominator += int(inferred_rate > MinAlpha)

        # Update precision
        if inferred_rate > MinAlpha:
            CurrentPrecision += int(true_rate < MinAlpha)

        # Calculate accuracy
        Accuracy = CurrentPrecision / CurrentAccuracyDenominator if CurrentAccuracyDenominator > 0 else 1.0
        accuracies.append(Accuracy)

    return accuracies

# Example inferred edge data file
inferred_edge_filename = "network.txt"

# Example true edge data file
true_edge_filename = "example-network.txt"

# Minimum alpha threshold
MinAlpha = 0.1

# Read inferred edge data
inferred_edge = read_edge_data(inferred_edge_filename)

# Read true edge data
true_edge = read_edge_data(true_edge_filename)

# Compute accuracy at all time steps
accuracies = compute_performance(inferred_edge, true_edge, MinAlpha)

# Display accuracies at all time steps
for t, accuracy in enumerate(accuracies, start=1):
    print(f"Accuracy at time step {t * 10}: {accuracy}")
