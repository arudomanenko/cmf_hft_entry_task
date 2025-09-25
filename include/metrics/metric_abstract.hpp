#pragma once

#include <memory>
#include <string>

namespace vault {
class Portfolio;
}

namespace vault::stats {

/**
 * @brief Abstract base class for portfolio metrics.
 *
 * Defines the interface for calculating a specific metric on a Portfolio.
 * Derived classes must implement the `calculate()` method.
 */
class MetricAbstract {
public:
  /// Unique pointer alias for convenience.
  using UPtr = std::unique_ptr<MetricAbstract>;

  /// Virtual destructor for proper cleanup in derived classes.
  virtual ~MetricAbstract() = default;

  /**
   * @brief Calculates the metric based on the given portfolio.
   *
   * Must be implemented by derived classes.
   *
   * @param portfolio Reference to the portfolio object.
   * @return Calculated metric value as double.
   */
  virtual double calculate(const Portfolio &portfolio) const = 0;

  /**
   * @brief Returns the name of the metric.
   *
   * Can be overridden in derived classes. Defaults to the protected `name_`
   * member.
   *
   * @return std::string Metric name.
   */
  inline virtual std::string getName() const { return name_; }

protected:
  /// Name of the metric, used for identification.
  std::string name_;
};

} // namespace vault::stats
