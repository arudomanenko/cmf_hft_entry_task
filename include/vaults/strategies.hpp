#pragma once

#include "types.hpp"
#include <cstdlib>
#include <memory>
#include <optional>

namespace vault {

/// @brief Base class for all trading strategies.
class StrategyBase {
public:
  using UPtr = std::unique_ptr<StrategyBase>;

  /// @brief Virtual destructor.
  virtual ~StrategyBase() = default;

  /**
   * @brief Called on each tick of LOB data to generate an order.
   *
   * Derived classes must implement this method to define strategy behavior.
   * It can return an order or std::nullopt if no action is needed.
   *
   * @return std::optional<common_types::Order> The order to execute, or
   * std::nullopt.
   */
  virtual std::optional<common_types::Order> on_tick() = 0;

  /**
   * @brief Sets the current LOB data for the strategy.
   *
   * This is typically called by the backtest or live engine before on_tick().
   *
   * @param data Reference to current LOB data.
   */
  void set_current_data(const raw_data::LOBData &data) { current_data_ = data; }

protected:
  /**
   * @brief Returns the mid price between the best bid and best ask.
   *
   * @return double Mid price.
   */
  inline double mid_price() const { return (best_bid() + best_ask()) / 2.0; }

  /**
   * @brief Returns the best bid price from the current LOB data.
   *
   * @return double Best bid price, or 0 if no bids are present.
   */
  double best_bid() const;

  /**
   * @brief Returns the best ask price from the current LOB data.
   *
   * @return double Best ask price, or 0 if no asks are present.
   */
  double best_ask() const;

  /**
   * @brief Creates a buy order with the given amount and optional price.
   *
   * @param amount Amount to buy.
   * @param price Optional limit price; 0 indicates market/IOC by default.
   * @return common_types::Order Buy order object.
   */
  common_types::Order create_buy_order(double amount, double price = .0) const;

  /**
   * @brief Creates a sell order with the given amount and optional price.
   *
   * @param amount Amount to sell.
   * @param price Optional limit price; 0 indicates market/IOC by default.
   * @return common_types::Order Sell order object.
   */
  common_types::Order create_sell_order(double amount, double price = .0) const;

protected:
  raw_data::LOBData current_data_; ///< Current LOB snapshot for this tick.
};
} // namespace vault
