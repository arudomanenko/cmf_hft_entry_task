#include "data_loader/args_parses.hpp"
#include "data_loader/csv_parser.hpp"
#include "execution/backtesing_engine.hpp"
#include "metrics/metrics_calculator.hpp"
#include "types.hpp"
#include "vaults/portfolio.hpp"
#include <memory>
#include <numeric>

namespace vault {

class SMACrossoverStrategy : public StrategyBase {
private:
  vault::Portfolio::SPtr portfolio_;
  size_t fast_window_{3};
  size_t slow_window_{7};
  std::vector<double> fast_prices_;
  std::vector<double> slow_prices_;
  int position_{0}; // -1 = short, 0 = flat, 1 = long
  double risk_per_trade_{0.01};
  size_t tick_count_{0};

public:
  explicit SMACrossoverStrategy(const vault::Portfolio::SPtr &portfolio)
      : portfolio_(portfolio) {}

  std::optional<common_types::Order> on_tick() override {
    tick_count_++;
    const double mid = mid_price();
    if (mid <= 0.0) {
      return std::nullopt;
    }

    fast_prices_.push_back(mid);
    slow_prices_.push_back(mid);

    if (fast_prices_.size() > fast_window_)
      fast_prices_.erase(fast_prices_.begin());
    if (slow_prices_.size() > slow_window_)
      slow_prices_.erase(slow_prices_.begin());

    if (fast_prices_.size() < fast_window_ ||
        slow_prices_.size() < slow_window_) {
      return std::nullopt;
    }

    double fast_sma =
        std::accumulate(fast_prices_.begin(), fast_prices_.end(), 0.0) /
        fast_prices_.size();
    double slow_sma =
        std::accumulate(slow_prices_.begin(), slow_prices_.end(), 0.0) /
        slow_prices_.size();

    double cash_to_use = portfolio_->get_cash_amount() * risk_per_trade_;
    double amount_to_trade = cash_to_use / best_ask();

    std::optional<common_types::Order> order;

    if (fast_sma > slow_sma) {
      if (position_ <= 0) {
        order = create_buy_order(amount_to_trade, best_ask());
        if (order) {
          position_ = 1;
        }
      }
    } else if (fast_sma < slow_sma) {
      if (position_ >= 0) {
        order = create_sell_order(amount_to_trade, best_bid());
        if (order) {
          position_ = -1;
        }
      }
    }

    if (!portfolio_->get_all_positions().empty()) {
      portfolio_->update_portfolio_value(mid);
    }

    return order;
  }
};

} // namespace vault

int main(int argc, char *argv[]) {
  ProgramArgs args = parse_arguments(argc, argv);

  data_loading::CSVParser csv_parser;

  auto lob_data = csv_parser.parse_lob(args.lob);

  auto portfolio = vault::Portfolio::create_portfolio();
  portfolio->set_amount(10000);
  portfolio->set_cash(10000);

  auto strat = std::make_unique<vault::SMACrossoverStrategy>(portfolio);

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