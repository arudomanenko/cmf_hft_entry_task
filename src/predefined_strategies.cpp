#include "vaults/predefined_strategies.hpp"

namespace vault {
StrategyFromTradesFile::StrategyFromTradesFile(
    const std::vector<raw_data::TradeData> &trades)
    : trades_(trades), trade_it_(trades_.begin()) {}

std::optional<common_types::Order> StrategyFromTradesFile::on_tick() {
  if (trade_it_ == trades_.end()) {
    return std::nullopt;
  }

  if (current_data_.local_timestamp >= trade_it_->local_timestamp) {
    common_types::Order order;
    order.price = trade_it_->price;
    order.amount = trade_it_->amount;
    order.side = trade_it_->side;
    order.order_type = execution::orders::OrderTypes::LimitIoc;

    ++trade_it_;
    return order;
  }

  return std::nullopt;
}
} // namespace vault