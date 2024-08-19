#include "price_reader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>


// Function to split a string by a delimiter
std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void readPrices(std::vector<double>& prices, std::string& fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file '" + fileName + "'. "
                                 "Please ensure the file exists and you have read permissions.");
    }

    std::string line;
    std::getline(file, line); // Skip header

    // Read prices from the CSV file
    while (std::getline(file, line)) {
        std::vector<std::string> row = split(line, ',');
        prices.push_back(std::stod(row[1])); // Assuming price is in the second column
    }

    // std::vector<double> lastFour(prices.end() - 4, prices.end());
    // prices = lastFour;

    file.close();
}