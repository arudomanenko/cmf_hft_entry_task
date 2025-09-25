#pragma once

#include "execution/orders.hpp"
#include "types.hpp"
#include "vaults/portfolio.hpp"
#include <unordered_map>
#include <vector>

namespace execution {

/**
 * @brief Simulates order execution in a market environment.
 *
 * The MarketEngine maintains a pool of pending orders, executes them according
 * to their type and current LOB data, and updates the portfolio accordingly.
 */
class MarketEngine {
public:
  /**
   * @brief Constructs a MarketEngine and initializes order execution policies.
   *
   * Registers executors for Market, Limit FOK, and Limit IOC orders.
   */
  MarketEngine();

  /**
   * @brief Adds a new order to the pending order pool.
   *
   * The order will be executed during the next tick based on LOB data.
   *
   * @param order Order to be added.
   */
  void add_order(const common_types::Order &order);

  /**
   * @brief Processes all pending orders against the given LOB snapshot.
   *
   * @param data Current LOB snapshot.
   * @param portfolio Shared pointer to the portfolio to update.
   * @return true If at least one order was executed.
   * @return false If no orders were executed.
   */
  bool tick(const raw_data::LOBData &data, vault::Portfolio::SPtr &portfolio);

  /**
   * @brief Executes a single order against the current LOB snapshot.
   *
   * @param order Order to execute.
   * @param data Current LOB snapshot.
   * @param portfolio Shared pointer to the portfolio to update.
   * @return true If the order was successfully executed.
   * @return false If the order could not be executed (e.g., no fills or
   * insufficient portfolio).
   *
   * @throws std::runtime_error If the order type is unsupported or the side is
   * undefined.
   */
  bool execute(const common_types::Order &order, const raw_data::LOBData &data,
               vault::Portfolio::SPtr &portfolio);

private:
  /// Mapping of order types to their respective executor objects.
  using OrdersExecitonPolicy =
      std::unordered_map<orders::OrderTypes,
                         orders::OrderExecutorAbstract::UPtr>;

  /// Execution policies for each order type (Market, Limit FOK, Limit IOC).
  OrdersExecitonPolicy orders_execution_policy_;

  /// Pool of orders waiting to be executed.
  std::vector<common_types::Order> pending_orders_;
};

} // namespace execution
