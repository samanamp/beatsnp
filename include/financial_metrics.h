#ifndef IRR_CALCULATOR_H
#define IRR_CALCULATOR_H

#include <vector>

namespace FinancialMetrics {
    // Function to calculate IRR using binary search
    double calculateIRR(const std::vector<double>& cashFlows);

    // Calculate Sharpe Ratio
    double calculateSharpeRatio(const std::vector<double>& returns, double riskFreeRate);

    // Calculate Sortino Ratio
    double calculateSortinoRatio(const std::vector<double>& returns, double riskFreeRate);

    // Calculate Sharpe Ratio
    double calculateSharpeRatio(const std::vector<double>& prices);

    // Calculate Sortino Ratio
    double calculateSortinoRatio(const std::vector<double>& prices);

}
#endif // IRR_CALCULATOR_H