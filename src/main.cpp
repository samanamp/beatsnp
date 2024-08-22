#include <iostream>
#include <price_reader.h>
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>
#include "financial_metrics.h"
#include <map>

// Helper function to calculate statistics for a single window
struct WindowStats {
    double annualizedIRR;
    double totalReturn;
};

WindowStats calculateWindowStats(const std::vector<double>& prices, int weeklyBudget) {
// Initialize cash flows
    std::vector<double> cashFlows;
    double totalInvested = 0.0;
    int shareCount = 0;

    int thisWeekInv = 0;
    double thisWeekInvDollar = 0.0;
    // Simulate weekly investments of $100
    for (size_t i = 0; i < prices.size(); ++i) {
        
        thisWeekInv = weeklyBudget/prices[i];
        thisWeekInvDollar = prices[i]*thisWeekInv;
        // std::cout << "week " << i << ", " << thisWeekInv << ", " << prices[i] << ", " << thisWeekInvDollar << "\n";
        cashFlows.push_back(-thisWeekInvDollar);  // Investment of $100
        totalInvested += thisWeekInvDollar;       // Total amount invested
        shareCount += thisWeekInv;
    }

    // Final portfolio value as the last cash flow
    double totalShares = shareCount;
    double finalPortfolioValue = totalShares * prices.back(); // Final portfolio value
    cashFlows.back() += finalPortfolioValue;

    // Calculate weekly IRR
    double weeklyIRR = FinancialMetrics::calculateIRR(cashFlows);
    // std::cout << "weekly IRR: " << weeklyIRR << "\n";
    // Convert weekly IRR to annualized IRR
    double annualizedIRR = std::pow(1.0 + weeklyIRR, 52) - 1;

    // Calculate total return
    double totalReturn = (shareCount*prices.back()) / totalInvested -1;

    // Calculate Sharpe and Sortino Ratios
    // double sharpeRatio = FinancialMetrics::calculateSharpeRatio(prices);
    // double sortinoRatio = FinancialMetrics::calculateSortinoRatio(prices);

    // Output the results
    std::cout << std::fixed << std::setprecision(7);
    // std::cout << "Total Return: " << totalReturn * 100 << "%" << std::endl;
    // std::cout << "Weekly IRR: " << weeklyIRR * 100 << "%" << std::endl;
    // std::cout << "Annualized IRR: " << annualizedIRR * 100 << "%" << std::endl;
    // std::cout << "Sharpe Ratio: " << sharpeRatio << std::endl;
    // std::cout << "Sortino Ratio: " << sortinoRatio << std::endl;

    return {annualizedIRR, totalReturn};
}

void addToThresholds(std::map<double, double>& thresholdMap, double annualizedIRR){
    std::vector<double> thresholds = {0.01, 0.03, 0.05, 0.07, 0.09};


    if (thresholdMap.empty()) {
        // Insert each threshold into the map with a value of 0.0
        for (const double& threshold : thresholds) {
            thresholdMap[threshold] = 0.0;
    }}

    // Compare the annualizedIRR with each threshold and increment if greater
    for (const double& threshold : thresholds) {
        if (annualizedIRR > threshold) {
            thresholdMap[threshold] += 1.0;
        }
    }
}

void printBacktestStats(std::vector<WindowStats> allWindowStats) {
    
    std::map<double, double> thresholdMap;

    for (WindowStats windowStat : allWindowStats) {
        addToThresholds(thresholdMap, windowStat.annualizedIRR);
    }

    // Convert counts to percentages
    double totalStats = static_cast<double>(allWindowStats.size());
    for (auto& pair : thresholdMap) {
        pair.second = (pair.second / totalStats) * 100.0;
    }

    // Example: print the map to verify
    for (const auto& pair : thresholdMap) {
        std::cout << std::fixed << std::setprecision(0);
        std::cout << "Threshold: " << pair.first *100 << "%" << ", of total: " << pair.second << "%" << std::endl;
    }

}

int main() {
    std::string fileName = "data/spy_avg_price-30-w.csv";
    int weeklyBudget = 1000;
    int windowSize = 5 * 52;

    std::vector<double> prices;
    readPrices(prices, fileName);

    std::vector<WindowStats> allWindowStats;

    // Perform rolling window analysis
    for (size_t i = 0; i <= prices.size() - windowSize; ++i) {
        std::vector<double> windowPrices(prices.begin() + i, prices.begin() + i + windowSize);
        allWindowStats.push_back(calculateWindowStats(windowPrices, weeklyBudget));
    }

    WindowStats overall = calculateWindowStats(prices, weeklyBudget);
    std::cout << "===========Overall stats (" << allWindowStats.size() << ")==========" << std::endl;
    printBacktestStats(allWindowStats);
    // Output the results
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "===========Overall Return==========" << std::endl;
    std::cout << "Total Return: " << overall.totalReturn * 100 << "%" << std::endl;
    std::cout << "Annualized IRR: " << overall.annualizedIRR * 100 << "%" << std::endl;

    return 0;
}
