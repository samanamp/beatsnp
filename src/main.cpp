#include <iostream>
#include <price_reader.h>
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>
#include "financial_metrics.h" 





int main() {
    std::string fileName = "data/qqq_avg_price-30-w.csv";
    int weeklyBudet = 1000;

    std::vector<double> prices;
    readPrices(prices, fileName);

    // Initialize cash flows
    std::vector<double> cashFlows;
    double totalInvested = 0.0;
    int shareCount = 0;

    int thisWeekInv = 0;
    double thisWeekInvDollar = 0.0;
    // Simulate weekly investments of $100
    for (size_t i = 0; i < prices.size(); ++i) {
        
        thisWeekInv = weeklyBudet/prices[i];
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
    std::cout << "weekly IRR: " << weeklyIRR << "\n";
    // Convert weekly IRR to annualized IRR
    double annualizedIRR = std::pow(1.0 + weeklyIRR, 52) - 1;

    // Calculate total return
    double totalReturn = (shareCount*prices.back()) / totalInvested -1;

    // Calculate Sharpe and Sortino Ratios
    double sharpeRatio = FinancialMetrics::calculateSharpeRatio(prices);
    double sortinoRatio = FinancialMetrics::calculateSortinoRatio(prices);

    // Output the results
    std::cout << std::fixed << std::setprecision(7);
    std::cout << "Total Return: " << totalReturn * 100 << "%" << std::endl;
    std::cout << "Weekly IRR: " << weeklyIRR * 100 << "%" << std::endl;
    std::cout << "Annualized IRR: " << annualizedIRR * 100 << "%" << std::endl;
    std::cout << "Sharpe Ratio: " << sharpeRatio << std::endl;
    std::cout << "Sortino Ratio: " << sortinoRatio << std::endl;

    return 0;
}
