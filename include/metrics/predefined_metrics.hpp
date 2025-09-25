#pragma once

#include "metrics/metric_abstract.hpp"

namespace vault::stats::prerefined {

/**
 * @brief Calculates realized Profit & Loss (PnL) from closed positions.
 */
class PnL final : public MetricAbstract {
public:
  /**
   * @brief Constructs PnL metric with a given name.
   *
   * @param name Metric name, defaults to "pnl".
   */
  explicit PnL(const std::string &name = "pnl");

  /**
   * @brief Calculates realized PnL for a portfolio.
   *
   * Iterates through the portfolio history and sums up realized PnL from
   * sell actions.
   *
   * @param portfolio Portfolio to calculate the metric for.
   * @return double Realized PnL value.
   */
  double calculate(const Portfolio &portfolio) const override;
};

/**
 * @brief Calculates maximum drawdown percentage for a portfolio.
 *
 * Maximum drawdown is defined as the maximum peak-to-trough decline over the
 * portfolio's value history.
 */
class MaxDrawdownMetric final : public MetricAbstract {
public:
  /**
   * @brief Constructs MaxDrawdownMetric with a given name.
   *
   * @param name Metric name, defaults to "max_drawdown".
   */
  explicit MaxDrawdownMetric(const std::string &name = "max_drawdown");

  /**
   * @brief Calculates maximum drawdown of the portfolio.
   *
   * @param portfolio Portfolio to calculate the metric for.
   * @return double Maximum drawdown as a percentage.
   */
  double calculate(const Portfolio &portfolio) const override;
};

/**
 * @brief Calculates Sharpe Ratio for a portfolio.
 *
 * If the portfolio has fewer than 2 value points or zero volatility, returns 0.
 */
class SharpeRatioMetric final : public MetricAbstract {
public:
  /**
   * @brief Constructs SharpeRatioMetric
   *
   * @param name Metric name, defaults to "sharpe_ratio".
   */
  explicit SharpeRatioMetric(const std::string &name = "sharpe_ratio");
  /**
   * @brief Calculates Sharpe Ratio for the given portfolio.
   *
   * @param portfolio Portfolio to calculate the metric for.
   * @return double Sharpe Ratio value
   */
  double calculate(const Portfolio &portfolio) const override;
};

/**
 * @brief Calculates total return of a portfolio.
 *
 * Total return measures the percentage gain or loss of the portfolio relative
 * to its initial value.
 */
class TotalReturnMetric final : public MetricAbstract {
private:
  double initial_value_; ///< Initial portfolio value used for calculation

public:
  /**
   * @brief Constructs TotalReturnMetric with initial portfolio value.
   *
   * @param initial_value Initial portfolio value (starting point for total
   * return).
   * @param name Metric name, defaults to "total_return".
   */
  explicit TotalReturnMetric(double initial_value,
                             const std::string &name = "total_return");
  /**
   * @brief Calculates total return of the portfolio.
   *
   * @param portfolio Portfolio to calculate the metric for.
   * @return double Total return in percentage.
   */
  double calculate(const Portfolio &portfolio) const override;
};
} // namespace vault::stats::prerefined
