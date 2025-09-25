#include "execution/market_engine.hpp"
#include "execution/orders.hpp"
#include "logging.hpp"
#include "types.hpp"
#include <algorithm>
#include <stdexcept>
#include <vector>

namespace execution {

MarketEngine::MarketEngine() {
  orders_execution_policy_.emplace(
      orders::OrderTypes::LimitFok,
      orders::create_executor<orders::LimitFokOrderExecutor>());
  orders_execution_policy_.emplace(
      orders::OrderTypes::LimitIoc,
      orders::create_executor<orders::LimitIocOrderExecutor>());
  orders_execution_policy_.emplace(
      orders::OrderTypes::Market,
      orders::create_executor<orders::MarketOrderExecutor>());
}

void MarketEngine::add_order(const common_types::Order &order) {
  logging::Logger::debug(
      "[ENGINE] Adding order to pool: side=", static_cast<int>(order.side),
      " amount=", order.amount, " price=", order.price);

  pending_orders_.push_back(order);
}

bool MarketEngine::tick(const raw_data::LOBData &data,
                        vault::Portfolio::SPtr &portfolio) {
  auto new_end = std::remove_if(pending_orders_.begin(), pending_orders_.end(),
                                [&](const common_types::Order &order) {
                                  return execute(order, data, portfolio);
                                });

  bool any_executed = (new_end != pending_orders_.end());
  pending_orders_.erase(new_end, pending_orders_.end());

  return any_executed;
}

bool MarketEngine::execute(const common_types::Order &order,
                           const raw_data::LOBData &data,
                           vault::Portfolio::SPtr &portfolio) {
  logging::Logger::debug(
      "[ENGINE] Strategy generated order: side=", static_cast<int>(order.side),
      " amount=", order.amount, " price=", order.price);

  auto order_executor_it = orders_execution_policy_.find(order.order_type);

  if (order_executor_it == orders_execution_policy_.end()) {
    throw std::runtime_error("Unsupported order type: there are only Market, "
                             "LimitFok and LimitIoc.");
  }

  const std::vector<common_types::ExecutionFill> fills =
      order_executor_it->second->execute_order(order, data);

  if (fills.empty()) {
    logging::Logger::debug("[ENGINE] No fills executed.");
    return false;
  }

  switch (order.side) {
  case common_types::Side::Buy:
    if (portfolio->can_buy(fills)) {
      logging::Logger::debug("[ENGINE] Portfolio CAN BUY. Executing...");
      portfolio->update_after_buy(fills);
      return true;
    } else {
      logging::Logger::debug("[ENGINE] Portfolio CANNOT BUY. Skipping.");
      return false;
    }

  case common_types::Side::Sell:
    if (portfolio->can_sell(fills)) {
      logging::Logger::debug("[ENGINE] Portfolio CAN SELL. Executing...");
      portfolio->update_after_sell(fills);
      return true;
    } else {
      logging::Logger::debug("[ENGINE] Portfolio CANNOT SELL. Skipping.");
      return false;
    }

  default:
    throw std::runtime_error("Undefined order side type");
  }

  return false;
}

} // namespace execution
