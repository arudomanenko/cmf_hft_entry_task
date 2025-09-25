#pragma once

#include "types.hpp"
#include <memory>
#include <vector>

namespace execution::orders {

/**
 * @brief Abstract base class for order execution.
 *
 * Provides interface for executing buy and sell orders given LOB data.
 */
class OrderExecutorAbstract {
public:
  /// Unique pointer alias for convenience.
  using UPtr = std::unique_ptr<OrderExecutorAbstract>;

  /**
   * @brief Executes an order by delegating to buy or sell method.
   *
   * @param order Order to execute.
   * @param data Current LOB snapshot.
   * @return Vector of execution fills.
   */
  std::vector<common_types::ExecutionFill>
  execute_order(const common_types::Order &order,
                const raw_data::LOBData &data);

  /**
   * @brief Executes a buy order.
   *
   * Must be implemented by derived classes.
   *
   * @param order Order to execute.
   * @param data Current LOB snapshot.
   * @return Vector of execution fills.
   */
  virtual std::vector<common_types::ExecutionFill>
  execute_buy_order(const common_types::Order &order,
                    const raw_data::LOBData &data) = 0;

  /**
   * @brief Executes a sell order.
   *
   * Must be implemented by derived classes.
   *
   * @param order Order to execute.
   * @param data Current LOB snapshot.
   * @return Vector of execution fills.
   */
  virtual std::vector<common_types::ExecutionFill>
  execute_sell_order(const common_types::Order &order,
                     const raw_data::LOBData &data) = 0;
};

/**
 * @brief Executes Limit FOK (Fill-Or-Kill) orders.
 *
 * Ensures the entire order is filled at the specified price or better, or not
 * at all.
 */
class LimitFokOrderExecutor final : public OrderExecutorAbstract {
public:
  std::vector<common_types::ExecutionFill>
  execute_buy_order(const common_types::Order &order,
                    const raw_data::LOBData &data) override;

  std::vector<common_types::ExecutionFill>
  execute_sell_order(const common_types::Order &order,
                     const raw_data::LOBData &data) override;
};

/**
 * @brief Executes Limit IOC (Immediate-Or-Cancel) orders.
 *
 * Fills as much as possible immediately at the specified price, canceling any
 * unfilled remainder.
 */
class LimitIocOrderExecutor final : public OrderExecutorAbstract {
public:
  std::vector<common_types::ExecutionFill>
  execute_buy_order(const common_types::Order &order,
                    const raw_data::LOBData &data) override;

  std::vector<common_types::ExecutionFill>
  execute_sell_order(const common_types::Order &order,
                     const raw_data::LOBData &data) override;
};

/**
 * @brief Executes Market orders.
 *
 * Fills the order immediately at the best available price until fully executed
 * or liquidity exhausted.
 */
class MarketOrderExecutor final : public OrderExecutorAbstract {
public:
  std::vector<common_types::ExecutionFill>
  execute_buy_order(const common_types::Order &order,
                    const raw_data::LOBData &data) override;

  std::vector<common_types::ExecutionFill>
  execute_sell_order(const common_types::Order &order,
                     const raw_data::LOBData &data) override;
};

/**
 * @brief Factory function to create a unique pointer to a given OrderExecutor
 * type.
 *
 * @tparam OrderExecutorT Concrete order executor class.
 * @return UPtr Unique pointer to the created executor.
 */
template <typename OrderExecutorT>
OrderExecutorAbstract::UPtr create_executor() {
  return std::make_unique<OrderExecutorT>();
}

} // namespace execution::orders
