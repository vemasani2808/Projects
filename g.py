import matplotlib.pyplot as plt
import random

# Generate timestamps from 10 to 100 with a fixed size interval of 10
timestamps = list(range(10, 101, 10))

# Calculate precision and recall with random fluctuations around 0.85
precision = [0.85 + random.uniform(-0.005, 0.005) for _ in timestamps]
recall = [0.85 + random.uniform(-0.005, 0.005) for _ in timestamps]

# Plot precision and recall
plt.plot(timestamps, precision, label='Precision', marker='o')
plt.plot(timestamps, recall, label='Recall', marker='o')

# Add labels and legend
plt.xlabel('Timestamps')
plt.ylabel('Score')
plt.title('Precision and Recall vs. Timestamps')
plt.legend()

# Show plot
plt.grid(True)
plt.show()
