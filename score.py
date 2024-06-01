import csv

# Input file path
input_file_path = 'example-cascade.txt'
output_file_path = 'selected_cascades.txt'
csv_file_path = 'scores.csv'  # Assuming this is your existing scores.csv file

# Create a list to store normalized scores
normalized_score_data = []

# Set the desired range for normalization
min_normalized_score = 0
max_normalized_score = 10

# Initialize variables to store min and max values
min_trust, max_trust = float('inf'), float('-inf')
min_engagementscore, max_engagementscore = float('inf'), float('-inf')

# Read data from the existing scores.csv file
with open(csv_file_path, 'r') as csv_file:
    csv_reader = csv.reader(csv_file)
    header = next(csv_reader)  # Skip the header row
    
    for row in csv_reader:
        target_node_id, trustscore, engagementscore = map(float, row)

        # Update min and max values for trust and engagementscore
        min_trust = min(min_trust, trustscore)
        max_trust = max(max_trust, trustscore)
        min_engagementscore = min(min_engagementscore, engagementscore)
        max_engagementscore = max(max_engagementscore, engagementscore)

# Read data again to calculate normalized scores
with open(csv_file_path, 'r') as csv_file:
    csv_reader = csv.reader(csv_file)
    header = next(csv_reader)  # Skip the header row
    
    for row in csv_reader:
        target_node_id, trustscore, engagementscore = map(float, row)

        # Normalize the scores to the range [0, 50]
        trustscore_normalized = min_normalized_score + (max_normalized_score - min_normalized_score) * (trustscore - min_trust) / (max_trust - min_trust)
        engagementscore_normalized = min_normalized_score + (max_normalized_score - min_normalized_score) * (engagementscore - min_engagementscore) / (max_engagementscore - min_engagementscore)

        # Append node ID, normalized trust score, and normalized engagement score to the list
        normalized_score_data.append([target_node_id, trustscore_normalized, engagementscore_normalized])

# Save the normalized data to a new CSV file
normalized_csv_file_path = 'normalized_scores.csv'
header = ['Node_ID', 'Normalized_Trust_Score', 'Normalized_Engagement_Score']

with open(normalized_csv_file_path, 'w', newline='') as normalized_csv_file:
    csv_writer = csv.writer(normalized_csv_file)
    csv_writer.writerow(header)
    csv_writer.writerows(normalized_score_data)

print(f"Normalized scores saved to {normalized_csv_file_path}")

