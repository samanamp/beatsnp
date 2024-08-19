#include "financial_metrics.h"
#include <cmath>
#include <numeric>
#include <stdexcept>

namespace FinancialMetrics {
    

    double calculateNPV(const std::vector<double>& cashFlows, double rate) {
        double npv = 0.0;
        for (size_t i = 0; i < cashFlows.size(); ++i) {
            npv += cashFlows[i] / std::pow(1.0 + rate, i);
        }
        return npv;
    }

    double calculateIRR(const std::vector<double>& cashFlows) {
        double low = -0.99, high = 0.99, irr = 0.0;
        while (high - low > 1e-10) {  // Precision threshold
            irr = (low + high) / 2.0;
            double npv = calculateNPV(cashFlows, irr);
            if (npv > 0) {
                low = irr;
            } else {
                high = irr;
            }
        }
        return irr;
    }

    // Function to calculate Sharpe Ratio
    double calculateSharpeRatio(const std::vector<double>& returns, double riskFreeRate) {
        double avgReturn = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
        double stdDev = 0.0;
        for (double ret : returns) {
            stdDev += std::pow(ret - avgReturn, 2);
        }
        stdDev = std::sqrt(stdDev / returns.size());
        return (avgReturn - riskFreeRate) / stdDev;
    }

    // Function to calculate Sortino Ratio
    double calculateSortinoRatio(const std::vector<double>& returns, double riskFreeRate) {
        double avgReturn = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
        double downsideDev = 0.0;
        for (double ret : returns) {
            if (ret < riskFreeRate) {
                downsideDev += std::pow(ret - riskFreeRate, 2);
            }
        }
        downsideDev = std::sqrt(downsideDev / returns.size());
        return (avgReturn - riskFreeRate) / downsideDev;
    }

        // Function to calculate Sharpe Ratio
    double calculateSharpeRatio(const std::vector<double>& prices) {
        // Calculate weekly returns
        std::vector<double> returns;
        for (size_t i = 1; i < prices.size(); ++i) {
            returns.push_back((prices[i] - prices[i - 1]) / prices[i - 1]);
        }

        // Assuming a risk-free rate of 0.01 (1% annual, converted to weekly)
        double riskFreeRate = std::pow(1.01, 1.0 / 52) - 1;

        // Calculate Sharpe and Sortino Ratios
        double sharpeRatio = FinancialMetrics::calculateSharpeRatio(returns, riskFreeRate);
        return sharpeRatio;
    }

    // Function to calculate Sortino Ratio
    double calculateSortinoRatio(const std::vector<double>& prices) {
        // Calculate weekly returns
        std::vector<double> returns;
        for (size_t i = 1; i < prices.size(); ++i) {
            returns.push_back((prices[i] - prices[i - 1]) / prices[i - 1]);
        }

        // Assuming a risk-free rate of 0.01 (1% annual, converted to weekly)
        double riskFreeRate = std::pow(1.01, 1.0 / 52) - 1;

        // Calculate Sharpe and Sortino Ratios
        double sortinoRatio = FinancialMetrics::calculateSortinoRatio(returns, riskFreeRate);
        return sortinoRatio;
    }


}