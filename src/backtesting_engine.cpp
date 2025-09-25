#include "execution/backtesing_engine.hpp"
#include "logging.hpp"

namespace execution {

bool BacktestEngine::run() {
  if (p_strategy_ == nullptr) {
    logging::Logger::debug("[BACKTEST] No strategy set.");
    return false;
  }

  logging::Logger::debug("[BACKTEST] Starting backtest over ", data_.size(),
                         " ticks.\n");

  for (size_t i{0}; i < data_.size(); ++i) {
    logging::Logger::debug("[BACKTEST] Tick #", i,
                           " ts=", data_[i].local_timestamp);
    if (!p_strategy_)
      return false;

    p_strategy_->set_current_data(data_[i]);

    const auto order = p_strategy_->on_tick();

    if (order.has_value()) {
      exec_engine_.add_order(*order);
    }
    exec_engine_.tick(data_[i], portfolio_);

    if (data_[i].bids.empty() || data_[i].asks.empty()) {
      return false;
    }

    const double price =
        (data_[i].bids[0].price + data_[i].asks[0].price) / 2.0;
    portfolio_->update_portfolio_value(price);

    logging::Logger::debug("------------");
  }

  return true;
}

} // namespace execution