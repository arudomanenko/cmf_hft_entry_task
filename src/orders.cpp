#include "execution/orders.hpp"
#include "logging.hpp"
#include <algorithm>

namespace execution::orders {

std::vector<common_types::ExecutionFill>
OrderExecutorAbstract::execute_order(const common_types::Order &order,
                                     const raw_data::LOBData &data) {
  if (order.side == common_types::Side::Buy) {
    return execute_buy_order(order, data);
  } else if (order.side == common_types::Side::Sell) {
    return execute_sell_order(order, data);
  }
  logging::Logger::debug("[EXEC] no fills");
  return {};
}

std::vector<common_types::ExecutionFill>
LimitFokOrderExecutor::execute_buy_order(const common_types::Order &order,
                                         const raw_data::LOBData &data) {
  logging::Logger::debug("[EXEC] FOK BUY amount=", order.amount,
                         " at price<=", order.price);

  if (data.asks.empty() || order.price < data.asks[0].price) {
    logging::Logger::debug("[EXEC][FOK BUY] No acceptable prices. No fill.");
    return {};
  }

  double available_amount = 0;
  for (const auto &ask : data.asks) {
    if (ask.price > order.price)
      break;
    available_amount += ask.amount;
    if (available_amount >= order.amount)
      break;
  }

  if (available_amount < order.amount) {
    logging::Logger::debug("[EXEC][FOK BUY] Not enough liquidity. No fill.");
    return {};
  }

  std::vector<common_types::ExecutionFill> fills;
  double remaining_amount = order.amount;

  for (const auto &ask : data.asks) {
    if (ask.price > order.price || remaining_amount <= 0)
      break;

    double amount_to_take = std::min(remaining_amount, ask.amount);
    fills.push_back({amount_to_take, ask.price});
    remaining_amount -= amount_to_take;
  }

  return fills;
}

std::vector<common_types::ExecutionFill>
LimitFokOrderExecutor::execute_sell_order(const common_types::Order &order,
                                          const raw_data::LOBData &data) {
  logging::Logger::debug("[EXEC] FOK SELL amount=", order.amount,
                         " at price>=", order.price);

  if (data.bids.empty() || order.price > data.bids[0].price) {
    logging::Logger::debug("[EXEC][FOK SELL] No acceptable prices. No fill.");
    return {};
  }

  double available_amount = 0;
  for (const auto &bid : data.bids) {
    if (bid.price < order.price)
      break;
    available_amount += bid.amount;
    if (available_amount >= order.amount)
      break;
  }

  if (available_amount < order.amount) {
    logging::Logger::debug("[EXEC][FOK SELL] Not enough liquidity. No fill.");
    return {};
  }

  std::vector<common_types::ExecutionFill> fills;
  double remaining_amount = order.amount;

  for (const auto &bid : data.bids) {
    if (bid.price < order.price || remaining_amount <= 0)
      break;

    double amount_to_take = std::min(remaining_amount, bid.amount);
    fills.push_back({amount_to_take, bid.price});
    logging::Logger::debug("[EXEC][FOK SELL] Fill: amount=", amount_to_take,
                           " @ price=", bid.price);
    remaining_amount -= amount_to_take;
  }

  return fills;
}

std::vector<common_types::ExecutionFill>
LimitIocOrderExecutor::execute_buy_order(const common_types::Order &order,
                                         const raw_data::LOBData &data) {
  std::vector<common_types::ExecutionFill> fills;
  logging::Logger::debug("[EXEC] IOC BUY amount=", order.amount,
                         " at price<=", order.price);

  if (data.asks.empty() || order.price < data.asks[0].price) {
    logging::Logger::debug("[EXEC][IOC BUY] No acceptable prices. No fill.");
    return {};
  }

  double remaining_amount = order.amount;
  for (const auto &ask : data.asks) {
    if (ask.price > order.price || remaining_amount <= 0)
      break;
    double amount_to_take = std::min(remaining_amount, ask.amount);
    fills.push_back({amount_to_take, ask.price});
    remaining_amount -= amount_to_take;
  }

  if (remaining_amount > 0) {
    logging::Logger::debug("[EXEC][IOC BUY] Partial fill. Remaining=",
                           remaining_amount);
  }

  return fills;
}

std::vector<common_types::ExecutionFill>
LimitIocOrderExecutor::execute_sell_order(const common_types::Order &order,
                                          const raw_data::LOBData &data) {
  std::vector<common_types::ExecutionFill> fills;
  logging::Logger::debug("[EXEC] IOC SELL amount=", order.amount,
                         " at price>=", order.price);

  if (data.bids.empty() || order.price > data.bids[0].price) {
    logging::Logger::debug("[EXEC][IOC SELL] No acceptable prices. No fill.");
    return {};
  }

  double remaining_amount = order.amount;
  for (const auto &bid : data.bids) {
    if (bid.price < order.price || remaining_amount <= 0)
      break;
    double amount_to_take = std::min(remaining_amount, bid.amount);
    fills.push_back({amount_to_take, bid.price});
    logging::Logger::debug("[EXEC][IOC SELL] Fill: amount=", amount_to_take,
                           " @ price=", bid.price);
    remaining_amount -= amount_to_take;
  }

  if (remaining_amount > 0) {
    logging::Logger::debug("[EXEC][IOC SELL] Partial fill. Remaining=",
                           remaining_amount);
  }

  return fills;
}

std::vector<common_types::ExecutionFill>
MarketOrderExecutor::execute_buy_order(const common_types::Order &order,
                                       const raw_data::LOBData &data) {
  std::vector<common_types::ExecutionFill> fills;
  logging::Logger::debug("[EXEC] MARKET BUY amount=", order.amount);

  if (data.asks.empty()) {
    logging::Logger::debug("[EXEC][MARKET BUY] No asks available. No fill.");
    return {};
  }

  double remaining_amount = order.amount;
  for (const auto &ask : data.asks) {
    if (remaining_amount <= 0)
      break;
    double amount_to_take = std::min(remaining_amount, ask.amount);
    fills.push_back({amount_to_take, ask.price});
    logging::Logger::debug("[EXEC][MARKET BUY] Fill: amount=", amount_to_take,
                           " @ price=", ask.price);
    remaining_amount -= amount_to_take;
  }

  if (remaining_amount > 0) {
    logging::Logger::debug(
        "[EXEC][MARKET BUY] Not enough liquidity. Order unfilled.");
    return {};
  }

  logging::Logger::debug("[EXEC][MARKET BUY] Order fully executed.");
  return fills;
}

std::vector<common_types::ExecutionFill>
MarketOrderExecutor::execute_sell_order(const common_types::Order &order,
                                        const raw_data::LOBData &data) {
  std::vector<common_types::ExecutionFill> fills;
  logging::Logger::debug("[EXEC] MARKET SELL amount=", order.amount);

  if (data.bids.empty()) {
    logging::Logger::debug("[EXEC][MARKET SELL] No bids available. No fill.");
    return {};
  }

  double remaining_amount = order.amount;
  for (const auto &bid : data.bids) {
    if (remaining_amount <= 0)
      break;
    double amount_to_take = std::min(remaining_amount, bid.amount);
    fills.push_back({amount_to_take, bid.price});
    logging::Logger::debug("[EXEC][MARKET SELL] Fill: amount=", amount_to_take,
                           " @ price=", bid.price);
    remaining_amount -= amount_to_take;
  }

  if (remaining_amount > 0) {
    logging::Logger::debug(
        "[EXEC][MARKET SELL] Not enough liquidity. Order partially filled.");
  } else {
    logging::Logger::debug("[EXEC][MARKET SELL] Order fully executed.");
  }

  return fills;
}

} // namespace execution::orders
