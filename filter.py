input_file_path = 'example-cascades.txt'
output_file_path = 'selected_cascades.txt'
start_line = 514

with open(input_file_path, 'r') as input_file, open(output_file_path, 'w') as output_file:
    # Skip lines until reaching the desired starting line
    for _ in range(start_line - 1):
        next(input_file)

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

        if 196 in website_ids:
            # Write selected cascades to the output file in the same format as input
            output_file.write(f"{meme_id};")
            for i, (website_id, timestamp) in enumerate(zip(website_ids, timestamps)):
                output_file.write(f"{website_id},{timestamp}")
                if i < len(website_ids) - 1:
                    output_file.write(',')

            output_file.write("\n")



