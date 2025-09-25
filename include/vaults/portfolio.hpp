#pragma once

#include "metrics/metrics_calculator.hpp"
#include "types.hpp"
#include <deque>
#include <memory>
#include <vector>

namespace vault {

/**
 * @brief Represents a trading portfolio, tracks cash, assets, positions and
 * history.
 */
class Portfolio {
public:
  using SPtr = std::shared_ptr<Portfolio>;
  using UPtr = std::unique_ptr<Portfolio>;

public:
  /**
   * @brief Default constructor initializing empty portfolio.
   */
  Portfolio() = default;

  static inline Portfolio::SPtr create_portfolio() {
    return std::make_shared<vault::Portfolio>();
  }

  /**
   * @brief Constructs portfolio with initial cash and optional asset amount.
   *
   * @param initial_cash Initial cash balance.
   * @param initial_amount Initial asset amount, default is 0.0.
   */
  explicit Portfolio(const double initial_cash,
                     const double initial_amount = .0);

  /** @brief Sets the cash balance. */
  inline void set_cash(const double cash) { cash_ = cash; }

  /** @brief Sets the asset amount. */
  inline void set_amount(const double amount) { asset_amount_ = amount; }

  /** @brief Returns current cash balance. */
  inline double get_cash_amount() const noexcept { return cash_; }

  /** @brief Returns trade history of the portfolio. */
  inline const std::vector<common_types::PositionInfo> &
  get_history() const noexcept {
    return trade_history_;
  }

  /** @brief Returns historical portfolio values. */
  inline const std::vector<double> &get_portfolio_values() const {
    return portfolio_values_;
  }

  /**
   * @brief Calculates current portfolio value using provided market price.
   *
   * @param current_price Current price of the asset.
   * @return double Total portfolio value.
   */
  inline double get_current_portfolio_value(double current_price) const {
    return cash_ + asset_amount_ * current_price;
  }

  /**
   * @brief Returns all open positions in the portfolio.
   *
   * @return std::vector<common_types::Lot> Vector of open lots.
   */
  std::vector<common_types::Lot> get_all_positions() const;

  /**
   * @brief Checks if the portfolio has enough cash to execute buy fills.
   *
   * @param fills Orders to buy.
   * @return true If cash is sufficient.
   * @return false Otherwise.
   */
  bool
  can_buy(const std::vector<common_types::ExecutionFill> &fills) const noexcept;

  /**
   * @brief Checks if the portfolio has enough assets to execute sell fills.
   *
   * @param fills Orders to sell.
   * @return true If asset amount is sufficient.
   * @return false Otherwise.
   */
  bool can_sell(
      const std::vector<common_types::ExecutionFill> &fills) const noexcept;

  /**
   * @brief Updates portfolio after a buy execution.
   *
   * Adjusts cash, asset amount, positions and trade history.
   *
   * @param fills Executed buy fills.
   */
  void update_after_buy(
      const std::vector<common_types::ExecutionFill> &fills) noexcept;

  /**
   * @brief Updates portfolio after a sell execution.
   *
   * Adjusts cash, asset amount, positions, trade history and calculates
   * realized PnL.
   *
   * @param fills Executed sell fills.
   */
  void update_after_sell(
      const std::vector<common_types::ExecutionFill> &fills) noexcept;

  /**
   * @brief Records portfolio value at a given price.
   *
   * @param current_price Current asset price.
   */
  void update_portfolio_value(const double current_price);

private:
  /**
   * @brief Helper to calculate realized PnL for a sell from existing positions.
   *
   * Uses FIFO logic to match lots and compute PnL.
   *
   * @param amount Amount being sold.
   * @param sell_price Execution price.
   * @return double Realized PnL from this sell.
   */
  double calculate_realized_pnl(double amount, double sell_price);

private:
  double cash_{.0};                                       ///< Cash balance
  double asset_amount_{.0};                               ///< Asset holdings
  std::vector<common_types::PositionInfo> trade_history_; ///< Buy/Sell history
  std::deque<common_types::Lot> positions_; ///< Open positions (FIFO)
  std::vector<double> portfolio_values_;    ///< Portfolio value history
};

} // namespace vault
