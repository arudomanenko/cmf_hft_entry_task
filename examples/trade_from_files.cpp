#include "data_loader/args_parses.hpp"
#include "data_loader/csv_parser.hpp"
#include "execution/backtesing_engine.hpp"
#include "metrics/metrics_calculator.hpp"
#include "vaults/portfolio.hpp"
#include "vaults/predefined_strategies.hpp"
#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
  ProgramArgs args = parse_arguments(argc, argv);

  data_loading::CSVParser csv_parser;

  auto lob_data = csv_parser.parse_lob(args.lob);
  auto trades_data = csv_parser.parse_trades(args.trades);

  auto portfolio = vault::Portfolio::create_portfolio();
  portfolio->set_amount(0);
  portfolio->set_cash(1000000);

  auto strat = std::make_unique<vault::StrategyFromTradesFile>(trades_data);
  vault::stats::MetricsCalculator metrics_calculator;

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