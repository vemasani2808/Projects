import csv

csv_file_path = 'node_labels.csv'

# Read existing labels from the CSV file into a dictionary
node_labels = {}
with open(csv_file_path, 'r') as csv_file:
    csv_reader = csv.reader(csv_file)
    header = next(csv_reader)  # Skip the header
    for row in csv_reader:
        node_id, label = int(row[0]), row[1]
        node_labels[node_id] = label

# Set the label of node 27 to 'positive'
node_labels[27] = 'positive'

# Write the updated labels back to the CSV file
with open(csv_file_path, 'w', newline='') as csv_file:
    csv_writer = csv.writer(csv_file)
    csv_writer.writerow(['node_id', 'label'])

    for node_id, label in node_labels.items():
        csv_writer.writerow([node_id, label])

# Now, the label of node 27 in the 'random_node_labels.csv' file is set to 'positive'.


