#pragma once

#include "metrics/metric_abstract.hpp"
#include <unordered_map>
#include <vector>

namespace vault {
class Portfolio;
}

namespace vault::stats {

/**
 * @brief Manager class for portfolio metrics calculation.
 *
 * Allows registering multiple metrics, calculating individual metrics,
 * and computing all registered metrics for a given portfolio.
 */
class MetricsCalculator {
public:
  /**
   * @brief Constructs a MetricsCalculator and registers default metrics.
   *
   * By default, registers predefined metrics such as PnL and MaxDrawdown.
   */
  MetricsCalculator();

  /**
   * @brief Registers a new metric.
   *
   * The metric is stored internally and can be used later for calculation.
   *
   * @param metric Unique pointer to a MetricAbstract-derived instance.
   */
  void register_metric(MetricAbstract::UPtr &&metric);

  /**
   * @brief Calculates all registered metrics for a given portfolio.
   *
   * @param portfolio Reference to the portfolio to evaluate.
   * @return std::unordered_map<std::string, double> Mapping from metric names
   * to their values.
   */
  std::unordered_map<std::string, double>
  calculate_all_metrics(const Portfolio &portfolio) const;

  /**
   * @brief Calculates a single metric by name.
   *
   * @param name Name of the metric to calculate.
   * @param portfolio Reference to the portfolio to evaluate.
   * @return double Calculated metric value, or -1 if the metric is not found.
   */
  double calculate_metric(const std::string &name,
                          const Portfolio &portfolio) const;

  /**
   * @brief Returns a list of all registered metric names.
   *
   * @return std::vector<std::string> Names of available metrics.
   */
  std::vector<std::string> get_available_metrics() const;

private:
  /// Mapping from metric name to unique pointer of MetricAbstract.
  std::unordered_map<std::string, MetricAbstract::UPtr> metrics_;
};

} // namespace vault::stats
