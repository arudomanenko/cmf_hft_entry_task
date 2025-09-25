#include "vaults/strategies.hpp"
#include "types.hpp"
#include <cstdlib>

namespace vault {
double StrategyBase::best_bid() const {
  if (current_data_.bids.empty())
    return .0;
  return current_data_.bids[0].price;
}

double StrategyBase::best_ask() const {
  if (current_data_.asks.empty())
    return .0;
  return current_data_.asks[0].price;
}

common_types::Order StrategyBase::create_buy_order(double amount,
                                                   double price) const {
  common_types::Order order;
  order.side = common_types::Side::Buy;
  order.amount = amount;
  order.price = (price > 0.0) ? price : best_ask();
  return order;
}

common_types::Order StrategyBase::create_sell_order(double amount,
                                                    double price) const {
  common_types::Order order;
  order.side = common_types::Side::Sell;
  order.amount = amount;
  order.price = (price > 0.0) ? price : best_bid();
  return order;
}
} // namespace vault