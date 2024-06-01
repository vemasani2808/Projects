import csv

# Replace 'your_file.csv' with the actual file path
file_path = 'your_file.csv'

# Initialize arrays for node ids, trust, and engagement scores
node_ids = []
trust_scores = []
engagement_scores = []

# Read the CSV file and populate the arrays
with open('normalized_scores.csv', 'r') as file:
    csv_reader = csv.reader(file)
    
    # Skip the header row if it exists
    next(csv_reader, None)
    
    for row in csv_reader:
        node_id, trust, engagement = row
        node_ids.append(int(float(node_id)))
        trust_scores.append(round(float(trust), 2))
        engagement_scores.append(round(float(engagement), 2))

# Print the arrays exactly as they look
print("Node IDs:", node_ids)
print("Trust Scores:", trust_scores)
print("Engagement Scores:", engagement_scores)

print(len(trust_scores))
print(len(engagement_scores))
