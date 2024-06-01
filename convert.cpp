#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main() {
    // Input and output file paths
    std::string inputFilePath = "nodes.txt";
    std::string outputFilePath = "nodes.csv";

    // Open input file
    std::ifstream inputFile(inputFilePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file: " << inputFilePath << std::endl;
        return 1;
    }

    // Open output file
    std::ofstream outputFile(outputFilePath);
    if (!outputFile.is_open()) {
        std::cerr << "Error opening output file: " << outputFilePath << std::endl;
        return 1;
    }

    // Write header to CSV file
    outputFile << "id,label\n";

    // Read each line from the input file and write to the CSV file with id as row number
    std::string line;
    int id = 0;
    while (std::getline(inputFile, line)) {
        // Replace comma with nothing
        size_t pos = line.find(',');
        if (pos != std::string::npos) {
            line.erase(pos, 1);
        }

        // Write to CSV file with id and label
        outputFile << id << "," << line << "\n";
        id++;
    }

    // Close files
    inputFile.close();
    outputFile.close();

    std::cout << "Conversion completed successfully. CSV file created: " << outputFilePath << std::endl;

    return 0;
}
