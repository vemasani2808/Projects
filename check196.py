website_ids_list = []
timestamps_list = []

source_count = 0
midcount = 0

with open('selected_cascades_with_196.txt', 'r') as file:
    for line in file:
        parts = line.strip().split(';')
        meme_id = int(parts[0])
        website_info = parts[1].split(',')
        
        # Extract the website ID and timestamp of the first node
        first_node = int(website_info[0])
        
        website_ids = [int(website_info[i]) for i in range(0, len(website_info), 2)]
        timestamps = [float(website_info[i]) for i in range(1, len(website_info), 2)]
        website_ids_list.extend(website_ids)
        timestamps_list.extend(timestamps)

        if first_node == 196:
            source_count += 1
        # Break out of the loop after finding the first line with 196
            
        else:

        # Create a mapping from website_ids to timestamps
            website_ids_to_timestamps = dict(zip(website_ids_list, timestamps_list))

        # Calculate midts using all lines
            midts = (timestamps_list[-1] + timestamps_list[0]) / 2

            ts_196 = website_ids_to_timestamps[196]

            if ts_196 <= midts:
                midcount += 1

print("Source Count:", source_count)
print("Midcount:", midcount)




    