#pragma once
#include <vector>

/// @brief Order types supported by the execution engine.
namespace execution::orders {
enum class OrderTypes {
  Market,   ///< Market order: execute at best available price.
  LimitFok, ///< Limit Fill-or-Kill: execute fully at limit price or cancel.
  LimitIoc  ///< Limit Immediate-or-Cancel: execute as much as possible at limit
            ///< price, cancel remainder.
};
} // namespace execution::orders

/// @brief Common trading types used by strategies and portfolio.
namespace common_types {

/// @brief Buy or Sell side of an order or trade.
enum class Side {
  Undefined, ///< No side defined.
  Sell,      ///< Sell order or trade.
  Buy        ///< Buy order or trade.
};

/// @brief Represents a trading order.
struct Order {
  Side side{Side::Undefined}; ///< Buy or Sell.
  execution::orders::OrderTypes order_type{
      ///< Order type.
      execution::orders::OrderTypes::Market};
  double price{.0};  ///< Price per unit.
  double amount{.0}; ///< Amount to buy or sell.
};

/// @brief Represents an execution fill of an order.
struct ExecutionFill {
  double amount{.0}; ///< Amount filled.
  double price{.0};  ///< Price at which the amount was filled.
};

/// @brief Lot of assets in the portfolio.
struct Lot {
  double entry_price{.0}; ///< Price at which the lot was acquired.
  double amount{.0};      ///< Quantity of the lot.
};

/// @brief Information about a closed or executed position.
struct PositionInfo {
  Side action_type;        ///< Buy or Sell.
  Lot lot;                 ///< Lot involved in the trade.
  double realised_pnl{.0}; ///< Realized profit or loss from this trade.
};

} // namespace common_types

/// @brief Raw market data structures.
namespace raw_data {

/// @brief Single entry in the order book.
struct OrderBookEntry {
  double price;  ///< Price level.
  double amount; ///< Available amount at this price.
};

/// @brief Snapshot of the limit order book at a given timestamp.
struct LOBData {
  long long local_timestamp; ///< Timestamp of the snapshot.
  std::vector<OrderBookEntry>
      asks; ///< Ask side of the book (sorted ascending).
  std::vector<OrderBookEntry>
      bids; ///< Bid side of the book (sorted descending).
};

/// @brief Trade record used for backtesting or historical replay.
struct TradeData {
  long long local_timestamp; ///< Timestamp of the trade.
  common_types::Side side;   ///< Buy or Sell.
  double price;              ///< Price at which trade occurred.
  double amount;             ///< Amount traded.
};

} // namespace raw_data
