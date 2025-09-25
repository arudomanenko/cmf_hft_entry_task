#include "data_loader/args_parses.hpp"
#include "data_loader/csv_parser.hpp"
#include "execution/backtesing_engine.hpp"
#include "metrics/metrics_calculator.hpp"
#include "vaults/portfolio.hpp"
#include "vaults/predefined_strategies.hpp"
#include <iostream>
#include <memory>

namespace vault {
class TotalReturnMetric final : public stats::MetricAbstract {
private:
  double initial_value_;

public:
  explicit TotalReturnMetric(double initial_value,
                             const std::string &name = "total_return")
      : initial_value_(initial_value) {
    name_ = name;
  }

  double calculate(const Portfolio &portfolio) const override {
    if (portfolio.get_portfolio_values().empty())
      return 0.0;
    double final_value = portfolio.get_portfolio_values().back();
    return (final_value - initial_value_) / initial_value_ * 100.0;
  }
};

} // namespace vault

int main(int argc, char *argv[]) {
  ProgramArgs args = parse_arguments(argc, argv);

  data_loading::CSVParser csv_parser;

  auto lob_data = csv_parser.parse_lob(args.lob);
  auto trades_data = csv_parser.parse_trades(args.trades);

  const double initial_cash{10000.};

  auto portfolio = vault::Portfolio::create_portfolio();

  portfolio->set_amount(100000.);
  portfolio->set_cash(initial_cash);

  auto strat = std::make_unique<vault::StrategyFromTradesFile>(trades_data);

  vault::stats::MetricsCalculator metrics_calculator;
  auto total_return_metric =
      std::make_unique<vault::TotalReturnMetric>(initial_cash);
  metrics_calculator.register_metric(std::move(total_return_metric));

  execution::BacktestEngine eng;
  eng.link_portfolio(portfolio);
  eng.add_data(lob_data);
  eng.set_strategy(std::move(strat));

  if (!eng.run()) {
    std::cout << "Backtest failure\n";
  }

  const auto metrics = metrics_calculator.calculate_all_metrics(*portfolio);

  for (const auto &m : metrics) {
    std::cout << m.first << " : " << m.second << std::endl;
  }
  std::cout << std::endl;

  return 0;
}