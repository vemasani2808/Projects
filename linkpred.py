import pandas as pd
import torch
from torch_geometric.data import Data
from torch_geometric.nn import SAGEConv
import torch.nn.functional as F
from sklearn.model_selection import train_test_split

# Load and preprocess data
df = pd.read_csv('output.csv', header=None)
edge_index = torch.tensor(df.values, dtype=torch.long).t().contiguous()
data = Data(edge_index=edge_index)

# Split the dataset
train_data, test_data = train_test_split(data.edge_index.t().numpy(), test_size=0.2)
train_edge_index = torch.tensor(train_data, dtype=torch.long).t().contiguous()
test_edge_index = torch.tensor(test_data, dtype=torch.long).t().contiguous()

# Define the GNN Model
class LinkPredictionGNN(torch.nn.Module):
    def __init__(self, in_channels, hidden_channels, out_channels):
        super(LinkPredictionGNN, self).__init__()
        self.conv1 = SAGEConv(in_channels, hidden_channels)
        self.conv2 = SAGEConv(hidden_channels, out_channels)

    def forward(self, x, edge_index):
        x = self.conv1(x, edge_index)
        x = F.relu(x)
        x = self.conv2(x, edge_index)
        return x

# Training the Model
model = LinkPredictionGNN(in_channels=1, hidden_channels=64, out_channels=32)
optimizer = torch.optim.Adam(model.parameters(), lr=0.01)
criterion = torch.nn.BCEWithLogitsLoss()

for epoch in range(100):
    optimizer.zero_grad()
    out = model(torch.ones(data.num_nodes, 1), train_edge_index)

    # Generate all possible edges for prediction
    all_edges = torch.cartesian_prod(torch.arange(data.num_nodes), torch.arange(data.num_nodes)).t()
    all_edges = all_edges[:, all_edges[0] != all_edges[1]]  # Remove self-loops

    # Predictions for all possible edges
    all_preds = torch.sigmoid(model(torch.ones(data.num_nodes, 1), all_edges))

    # Save predicted edges to CSV
    predicted_edges = all_edges[all_preds.squeeze() > 0.5].numpy()
    predicted_df = pd.DataFrame(predicted_edges, columns=['Source', 'Target'])
    predicted_df.to_csv('predicted_links.csv', index=False)

    loss = criterion(out[train_edge_index[0]], torch.ones(train_edge_index.size(1)))
    loss.backward()
    optimizer.step()

    if epoch % 10 == 0:
        print(f'Epoch {epoch}/{100}, Loss: {loss.item()}')

# Evaluation
with torch.no_grad():
    test_out = model(torch.ones(data.num_nodes, 1), test_edge_index)
    test_pred = torch.sigmoid(test_out[test_edge_index[0]])

    # Implement your evaluation metrics as needed
