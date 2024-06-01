# Load the updated DataFrame with link predictions
import pandas as pd
df = pd.read_csv('your_data_with_gnn_predictions.csv')

# Find the minimum and maximum values of 'link_prediction' and 'target'
min_link_prediction = df['link_prediction'].min()
max_link_prediction = df['link_prediction'].max()
min_target = df['target'].min()
max_target = df['target'].max()

print(f"Min link_prediction: {min_link_prediction}")
print(f"Max link_prediction: {max_link_prediction}")
print(f"Min target: {min_target}")
print(f"Max target: {max_target}")