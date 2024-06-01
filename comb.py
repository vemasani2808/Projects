import pandas as pd

# Assuming you have a CSV file named 'input_file.csv'
input_file_path = 'out.csv'

# Read the CSV file
data = pd.read_csv(input_file_path)

# Add a new column 'Label' with values from 1 to the length of the data
data['ID'] = range(1, len(data) + 1)

# Write the data with labels to a new CSV file
data.to_csv('output_file_with_labels.csv', index=False)

print("CSV file with labels created successfully: output_file_with_labels.csv")

