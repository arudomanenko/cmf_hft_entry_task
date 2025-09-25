#pragma once

#include "execution/market_engine.hpp"
#include "vaults/portfolio.hpp"
#include "vaults/strategies.hpp"
#include <vector>

namespace execution {

/**
 * @brief Engine for backtesting trading strategies over historical LOB data.
 *
 * This class simulates a trading environment by feeding LOB snapshots
 * into a user-defined strategy and updating the portfolio based on executed
 * orders.
 */
class BacktestEngine {
public:
  /**
   * @brief Links a portfolio to the backtest engine.
   *
   * The portfolio will be updated according to strategy actions and market
   * events during backtesting.
   *
   * @param portfolio Shared pointer to a Portfolio instance.
   */
  inline void link_portfolio(vault::Portfolio::SPtr &portfolio) {
    portfolio_ = portfolio;
  }

  /**
   * @brief Sets the historical LOB data for the backtest.
   *
   * @param lob_data Vector of LOB snapshots (raw_data::LOBData).
   */
  inline void add_data(const std::vector<raw_data::LOBData> &lob_data) {
    data_ = lob_data;
  }

  /**
   * @brief Assigns a trading strategy to the backtest engine.
   *
   * The engine will call `on_tick()` of the strategy for each LOB snapshot.
   *
   * @param p_strategy Unique pointer to a StrategyBase instance.
   */
  inline void set_strategy(vault::StrategyBase::UPtr &&p_strategy) {
    p_strategy_ = std::move(p_strategy);
  }

  /**
   * @brief Runs the backtest over all loaded LOB data.
   *
   * @return true If the backtest completed successfully.
   * @return false If no strategy was set, LOB data is invalid, or an early exit
   * occurred.
   */
  bool run();

private:
  /// The market simulation engine responsible for executing orders.
  MarketEngine exec_engine_;

  /// Shared pointer to the portfolio being managed in the backtest.
  vault::Portfolio::SPtr portfolio_;

  /// Unique pointer to the trading strategy being tested.
  vault::StrategyBase::UPtr p_strategy_;

  /// Historical LOB data used for backtesting.
  std::vector<raw_data::LOBData> data_;
};

} // namespace execution
