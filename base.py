import numpy as np
import csv
# Input file path

input_file_path = 'example-cascade.txt'
output_file_path = 'selected_cascades.txt'

# Create a list to store trust and engagement scores
score_data = []

# Iterate through each node ID from 0 to 512
for target_node_id in range(513):
    with open(input_file_path, 'r') as input_file, open(output_file_path, 'w') as output_file:
        for line in input_file:
            parts = line.strip().split(';')

            # Check if the line has the expected structure
            if len(parts) < 2:
                print(f"Skipping line {line.strip()} - Unexpected format")
                continue

            meme_id = int(parts[0])
            website_info = parts[1].split(',')

            # Ensure there are pairs of website_ids and timestamps
            if len(website_info) % 2 != 0:
                print(f"Skipping line {line.strip()} - Uneven number of elements")
                continue

            website_ids = [int(website_info[i]) for i in range(0, len(website_info), 2)]
            timestamps = [float(website_info[i]) for i in range(1, len(website_info), 2)]

            if target_node_id in website_ids:
                # Write selected cascades to the output file in the same format as input
                output_file.write(f"{meme_id};")
                for i, (website_id, timestamp) in enumerate(zip(website_ids, timestamps)):
                    output_file.write(f"{website_id},{timestamp}")
                    if i < len(website_ids) - 1:
                        output_file.write(',')

                output_file.write("\n")
            
    source_count=0
    midcount=0

    website_ids_list = []
    timestamps_list = []

    with open('selected_cascades.txt', 'r') as file:
        total_appearances = sum(1 for line in file)
    
    #print(f"Selected cascades for Node {target_node_id} written to {output_file_path}")
    with open('selected_cascades.txt', 'r') as file:
        finalts=[]
        for line in file:
            parts = line.strip().split(';')
            meme_id = int(parts[0])
            website_info = parts[1].split(',')

            website_ids = [int(website_info[i]) for i in range(0, len(website_info), 2)]
            timestamps = [float(website_info[i]) for i in range(1, len(website_info), 2)]
        
            # Extract the website ID and timestamp of the first node
            first_node = int(website_info[0])
        
           
            website_ids_list.extend(website_ids)
            timestamps_list.extend(timestamps)

            # Create a mapping from website_ids to timestamps
            website_ids_to_timestamps = dict(zip(website_ids_list, timestamps_list))

            if first_node == target_node_id:
                source_count += 1
            # Break out of the loop after finding the first line with 196
            
            else:

                # Create a mapping from website_ids to timestamps
                #website_ids_to_timestamps = dict(zip(website_ids_list, timestamps_list))

                # Calculate midts using all lines
                midts = (timestamps_list[-1] + timestamps_list[0]) / 2

                ts_196 = website_ids_to_timestamps[target_node_id]

                if ts_196 <= midts:
                    midcount += 1
            ts_196 = website_ids_to_timestamps[target_node_id]
            finalts.append(ts_196)

            
    base_score=0.1
    trustscore =base_score+ (0.8)*source_count+(0.2)*midcount
    #print(trustscore)

    if(len(finalts)>1):

        # Random array of time steps (replace this with your actual data)
        time_steps = np.array(finalts)

        # Calculate Time Differences
        time_diff = np.diff(time_steps)

        # Find the Average Time Difference
        average_time_diff = np.mean(time_diff)

        # Calculate Variance
        variance = np.var(time_diff)
    else:
        variance=0

    csv_file_path = 'node_labels.csv'

    # Create an empty dictionary to store node labels
    node_labels = {}

    # Read data from the CSV file and populate the dictionary
    with open(csv_file_path, 'r') as csv_file:
        csv_reader = csv.reader(csv_file)
    
        # Skip the header row
        next(csv_reader)

        for row in csv_reader:
            node_id, label = int(row[0]), row[1]
            node_labels[node_id] = label

    label= node_labels[target_node_id]

    # Check if the label is positive or negative
    if label == 'positive':
        prob=1.2
    elif label== 'negative':
        prob=0.8
    else:
        prob=1.0



    engagementscore=(0.7)*total_appearances+(0.2)*variance+(0.1)*prob
    #print(engagementscore)
    score_data.append([target_node_id, trustscore, engagementscore])
    
# Save the data to a CSV file
csv_file_path = 'scores.csv'
header = ['Node_ID', 'Trust_Score', 'Engagement_Score']

with open(csv_file_path, 'w', newline='') as csv_file:
    csv_writer = csv.writer(csv_file)
    csv_writer.writerow(header)
    csv_writer.writerows(score_data)

print(f"Scores saved to {csv_file_path}")
