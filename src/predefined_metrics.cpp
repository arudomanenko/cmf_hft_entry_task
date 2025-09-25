#include "metrics/predefined_metrics.hpp"

#include "types.hpp"
#include "vaults/portfolio.hpp"
#include <cmath>
#include <numeric>

namespace vault::stats::prerefined {

PnL::PnL(const std::string &name) { name_ = name; }

double PnL::calculate(const Portfolio &portfolio) const {
  double realized_pnl = .0;

  const auto &history = portfolio.get_history();
  for (const auto &trade : history) {
    if (trade.action_type == common_types::Side::Sell) {
      realized_pnl += trade.realised_pnl;
    }
  }

  return realized_pnl;
}

MaxDrawdownMetric::MaxDrawdownMetric(const std::string &name) { name_ = name; }

double MaxDrawdownMetric::calculate(const Portfolio &portfolio) const {
  const auto &portfolio_values = portfolio.get_portfolio_values();
  if (portfolio_values.empty())
    return 0.0;

  double max_peak = portfolio_values[0];
  double max_drawdown = .0;

  for (double value : portfolio_values) {
    if (value > max_peak) {
      max_peak = value;
    }

    double drawdown = (max_peak - value) / max_peak;
    if (drawdown > max_drawdown) {
      max_drawdown = drawdown;
    }
  }

  return max_drawdown * 100.0;
}

SharpeRatioMetric::SharpeRatioMetric(const std::string &name) { name_ = name; }

double SharpeRatioMetric::calculate(const Portfolio &portfolio) const {
  const auto &values = portfolio.get_portfolio_values();
  if (values.size() < 2)
    return 0.0;

  std::vector<double> returns;
  returns.reserve(values.size() - 1);
  for (size_t i = 1; i < values.size(); ++i) {
    returns.push_back((values[i] - values[i - 1]) / values[i - 1]);
  }

  const double mean =
      std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
  double variance = 0.0;
  for (double r : returns)
    variance += (r - mean) * (r - mean);
  variance /= returns.size();
  const double stddev = std::sqrt(variance);

  if (stddev < 1e-10)
    return 0.0;

  return mean / stddev;
}

TotalReturnMetric::TotalReturnMetric(double initial_value,
                                     const std::string &name)
    : initial_value_(initial_value) {
  name_ = name;
}

double TotalReturnMetric::calculate(const Portfolio &portfolio) const {
  const auto values = portfolio.get_portfolio_values();
  if (values.empty())
    return 0.0;

  double final_value = values.back();
  return (final_value - initial_value_) / initial_value_ * 100.0;
}
} // namespace vault::stats::prerefined
