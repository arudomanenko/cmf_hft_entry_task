#include "metrics/metrics_calculator.hpp"
#include "metrics/predefined_metrics.hpp"
#include "vaults/portfolio.hpp"
#include <unordered_map>

namespace vault::stats {
MetricsCalculator::MetricsCalculator() {
  register_metric(std::make_unique<stats::prerefined::PnL>());
  register_metric(std::make_unique<stats::prerefined::MaxDrawdownMetric>());
  register_metric(std::make_unique<stats::prerefined::SharpeRatioMetric>());
}

void MetricsCalculator::register_metric(MetricAbstract::UPtr &&metric) {
  metrics_[metric->getName()] = std::move(metric);
}

std::unordered_map<std::string, double>
MetricsCalculator::calculate_all_metrics(const Portfolio &portfolio) const {
  std::unordered_map<std::string, double> res;
  for (const auto &name_n_metric : metrics_) {
    res[name_n_metric.first] = calculate_metric(name_n_metric.first, portfolio);
  }
  return res;
}

double MetricsCalculator::calculate_metric(const std::string &name,
                                           const Portfolio &portfolio) const

{
  const auto metric_it = metrics_.find(name);
  if (metric_it == metrics_.end()) {
    return -1.;
  }

  return metric_it->second->calculate(portfolio);
}

std::vector<std::string> MetricsCalculator::get_available_metrics() const {
  std::vector<std::string> names;
  for (const auto &[name, _] : metrics_) {
    names.push_back(name);
  }
  return names;
}
} // namespace vault::stats