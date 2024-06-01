#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <iterator>
#include <algorithm>
#include <unordered_map>
#include <vector>

// Struct to represent an entry in the unordered_map
struct EdgeEntry {
    std::string sourceNode;
    std::string targetNode;
    double edgeRate;
    int timestamp; // New field for timestamp

    // Constructor for convenience
    EdgeEntry(const std::string& source, const std::string& target, double rate)
        : sourceNode(source), targetNode(target), edgeRate(rate), timestamp(50) {}
};

// Function to compare EdgeEntry objects for sorting
bool compareEdgeEntries(const EdgeEntry& entry1, const EdgeEntry& entry2) {
    // Compare by sourceNode first
    if (entry1.sourceNode != entry2.sourceNode) {
        return entry1.sourceNode < entry2.sourceNode;
    }
    // If sourceNode is the same, compare by targetNode
    return entry1.targetNode < entry2.targetNode;
}

int main() {
    // Specify the input file path
    std::string inputFilePath = "new_network.txt";

    // Specify the output CSV file path
    std::string outputFilePath = "output5.csv";

    // Open the input file
    std::ifstream inputFile(inputFilePath);

    // Check if the input file is open
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file: " << inputFilePath << std::endl;
        return 1;
    }

    // Hash map to store the extracted information
    std::unordered_map<std::string, double> edgeMap;

    std::string inputSequence;
    // Loop through each line in the input file
    while (std::getline(inputFile, inputSequence)) {
        // Create a string stream from the current input line
        std::istringstream ss(inputSequence);

        // List to store extracted values
        std::list<std::string> values;

        // Tokenize the string using getline and comma as the delimiter
        std::string token;
        while (std::getline(ss, token, ',')) {
            values.push_back(token);
        }

        // Check if there are at least two values in the list
        if (values.size() >= 2) {
            auto sourceNode = values.front();
            auto targetNode = *std::next(values.begin(), 1);

            // Value to find
            std::string targetValue = "50.000000";

            // Check if the target value is present in the list
            auto it = std::find<std::list<std::string>::iterator>(values.begin(), values.end(), targetValue);

            if (it != values.end()) {
                // Check if there's a next value
                auto nextIt = std::next(it, 1);
                if (nextIt != values.end()) {
                    // Store source node, target node, edge rate, and timestamp in the unordered_map
                    edgeMap[sourceNode + "," + targetNode] = std::stod(*nextIt);
                }
            }
        }
    }

    // Close the input file
    inputFile.close();

    // Vector to store the entries for sorting
    std::vector<EdgeEntry> edgeEntries;

    // Populate the vector with entries from the unordered_map
    for (const auto& entry : edgeMap) {
        std::istringstream ss(entry.first);
        std::string sourceNode, targetNode;
        std::getline(ss, sourceNode, ',');
        std::getline(ss, targetNode, ',');

        edgeEntries.emplace_back(sourceNode, targetNode, entry.second);
    }

    // Sort the vector based on the specified criteria
    std::sort(edgeEntries.begin(), edgeEntries.end(), compareEdgeEntries);

    // Open the output CSV file
    std::ofstream outputFile(outputFilePath);

    // Check if the output file is open
    if (!outputFile.is_open()) {
        std::cerr << "Error opening output file: " << outputFilePath << std::endl;
        return 1;
    }

    // Write the header to the CSV file, including the new timestamp column
    outputFile << "Source Node,Target Node,Edge Rate,Timestamp" << std::endl;

    // Write the sorted contents of the vector to the CSV file
    for (const auto& entry : edgeEntries) {
        outputFile << entry.sourceNode << "," << entry.targetNode << ","
                   << entry.edgeRate << "," << entry.timestamp << std::endl;
    }

    // Close the output file
    outputFile.close();

    std::cout << "Data exported and sorted to CSV file: " << outputFilePath << std::endl;

    return 0;
}
