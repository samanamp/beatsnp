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

WindowStats calculateWindowStats(const std::vector<double>& prices, int weeklyBudget,
const std::vector<std::pair<double, int>>& thresholds) {
// Initialize cash flows
    std::vector<double> cashFlows;
    double totalInvested = 0.0;
    int shareCount = 0;
    double maxPrice = 0.0;

    int thisWeekInv = 0;
    double thisWeekInvDollar = 0.0;
    // Simulate weekly investments of $100
    for (size_t i = 0; i < prices.size(); ++i) {

        // Track the maximum price so far
        if (prices[i] > maxPrice) {
            maxPrice = prices[i];
        }

        // Calculate the percentage drop from the max price
        double dropPercentage = (maxPrice - prices[i]) / maxPrice * 100;

        // Determine investment multiplier
        double multiplier = 1.0; // Default multiplier (1x)

        for (const auto& threshold : thresholds) {
            if (dropPercentage >= threshold.first) {
                multiplier = threshold.second; // Set multiplier based on threshold
            }
        }
        
        thisWeekInv = (weeklyBudget * multiplier)/prices[i];
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
    std::vector<double> thresholds = {0.01, 0.03, 0.05, 0.07, 0.09, 0.11, 0.13, 0.15};


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

void findTopStrategies(const std::vector<double>& prices, int weeklyBudget) {
    std::vector<std::tuple<double, std::vector<std::pair<double, int>>, WindowStats>> results;

    // Define ranges for percentages and multipliers
    std::vector<double> percentages = {1.0, 2.5, 5.0, 7.5, 10.0, 15, 20, 25, 30, 35, 40};
    std::vector<int> multipliers = {2, 3, 4, 5, 7, 9, 10, 12, 15};

    // Test all combinations of thresholds and multipliers
    for (double p1 : percentages) {
        for (int m1 : multipliers) {
            for (double p2 : percentages) {
                if (p2 <= p1) continue; // Ensure unique and ordered thresholds
                for (int m2 : multipliers) {
                    for (double p3 : percentages) {
                        if (p3 <= p2) continue;
                        for (int m3 : multipliers) {
                            for (double p4 : percentages) {
                                if (p4 <= p3) continue;
                                for (int m4 : multipliers) {
                                    std::vector<std::pair<double, int>> thresholds = {{p1, m1}, {p2, m2}, {p3, m3}, {p4, m4}};
                                    WindowStats stats = calculateWindowStats(prices, weeklyBudget, thresholds);
                                    results.emplace_back(stats.totalReturn, thresholds, stats);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Sort results based on total return
    std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) > std::get<0>(b);
    });

    // Print top 5 strategies
    std::cout << "Top 5 Investment Strategies based on Return:\n";
    for (size_t i = 0; i < std::min(results.size(), size_t(5)); ++i) {
        const auto& [returnVal, thresholds, stats] = results[i];
        std::cout << "Strategy " << (i + 1) << ": Return = " << returnVal * 100 << "%, Thresholds: ";
        for (const auto& [percent, multiplier] : thresholds) {
            std::cout << "[" << percent << "% -> " << multiplier << "x] ";
        }
        std::cout << "Annualized IRR = " << stats.annualizedIRR * 100 << "%\n";
    }
}


int main() {
    std::string fileName = "data/qqq_avg_price-30-w.csv";
    int weeklyBudget = 1000;
    // std::vector<std::pair<double, int>> thresholds = {{5.0, 1}, {7.5, 1}, {10.0, 1}};
    std::vector<std::pair<double, int>> thresholds = {{2.0, 2}, {5.0, 2}, {7.5, 3}, {10.0, 5}};

    // int windowSize = 5 * 52;

    std::vector<double> prices;
    readPrices(prices, fileName);

    findTopStrategies(prices, weeklyBudget);

    // std::vector<WindowStats> allWindowStats;

    // // Perform rolling window analysis
    // for (size_t i = 0; i <= prices.size() - windowSize; ++i) {
    //     std::vector<double> windowPrices(prices.begin() + i, prices.begin() + i + windowSize);
    //     allWindowStats.push_back(calculateWindowStats(windowPrices, weeklyBudget, thresholds));
    // }

    // WindowStats overall = calculateWindowStats(prices, weeklyBudget, thresholds);
    // std::cout << "===========Overall stats (" << allWindowStats.size() << ")==========" << std::endl;
    // printBacktestStats(allWindowStats);
    // // Output the results
    // std::cout << std::fixed << std::setprecision(2);
    // std::cout << "===========Overall Return==========" << std::endl;
    // std::cout << "Total Return: " << overall.totalReturn * 100 << "%" << std::endl;
    // std::cout << "Annualized IRR: " << overall.annualizedIRR * 100 << "%" << std::endl;

    return 0;
}
