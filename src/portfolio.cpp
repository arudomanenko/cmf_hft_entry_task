#include "vaults/portfolio.hpp"
#include "types.hpp"

#include "logging.hpp"
#include <vector>

constexpr double EPS_D = 1e-10;

namespace vault {
Portfolio::Portfolio(const double initial_cash, const double initial_amount) {
  set_cash(initial_cash);
  set_amount(initial_amount);
}

bool Portfolio::can_buy(
    const std::vector<common_types::ExecutionFill> &fills) const noexcept {
  double total_cost = 0.0;
  for (const auto &f : fills) {
    total_cost += f.amount * f.price;
  }
  logging::Logger::debug(
      "[PORTFOLIO] Can buy? Need=", std::to_string(total_cost),
      " Cash=", std::to_string(cash_));
  return cash_ >= total_cost;
}

bool Portfolio::can_sell(
    const std::vector<common_types::ExecutionFill> &fills) const noexcept {
  double total_amount = 0.0;
  for (const auto &f : fills) {
    total_amount += f.amount;
  }
  logging::Logger::debug(
      "[PORTFOLIO] Can sell? Need=", std::to_string(total_amount),
      " Assets=", std::to_string(asset_amount_));
  return asset_amount_ >= total_amount;
}

void Portfolio::update_after_buy(
    const std::vector<common_types::ExecutionFill> &fills) noexcept {
  for (const auto &f : fills) {
    common_types::Lot l{f.price, f.amount};
    positions_.push_back(l);

    cash_ -= f.amount * f.price;
    asset_amount_ += f.amount;
    trade_history_.push_back({common_types::Side::Buy, l, .0});

    logging::Logger::debug(
        "[PORTFOLIO][BUY] Bought amount=", std::to_string(f.amount), " @ ",
        std::to_string(f.price), " Cash now=", std::to_string(cash_),
        " Assets now=", std::to_string(asset_amount_));
  }
}

void Portfolio::update_after_sell(
    const std::vector<common_types::ExecutionFill> &fills) noexcept {
  for (const auto &f : fills) {
    const auto realized_pnl = calculate_realized_pnl(f.amount, f.price);

    cash_ += f.amount * f.price;
    asset_amount_ -= f.amount;
    trade_history_.push_back(
        {common_types::Side::Sell, {f.price, f.amount}, realized_pnl});

    logging::Logger::debug(
        "[PORTFOLIO][SELL] Sold amount=", std::to_string(f.amount), " @ ",
        std::to_string(f.price), " Cash now=", std::to_string(cash_),
        " Assets now=", std::to_string(asset_amount_),
        " RealizedPnL=", std::to_string(realized_pnl));
  }
}

std::vector<common_types::Lot> Portfolio::get_all_positions() const {
  return std::vector<common_types::Lot>(positions_.begin(), positions_.end());
}

void Portfolio::update_portfolio_value(const double current_price) {
  double current_value = cash_ + asset_amount_ * current_price;
  portfolio_values_.push_back(current_value);
}

double Portfolio::calculate_realized_pnl(double amount, double sell_price) {
  double realised_pnl{.0};

  while (amount > 0 && !positions_.empty()) {
    auto &front_lot = positions_.front();
    double sell_from_this_lot = std::min(amount, front_lot.amount);

    double lot_pnl = (sell_price - front_lot.entry_price) * sell_from_this_lot;
    realised_pnl += lot_pnl;

    logging::Logger::debug(
        "[PORTFOLIO][PNL] Lot entry=", std::to_string(front_lot.entry_price),
        " Sell=", sell_price, " Amount=", std::to_string(sell_from_this_lot),
        " PnL=", std::to_string(lot_pnl));

    front_lot.amount -= sell_from_this_lot;
    amount -= sell_from_this_lot;

    if (front_lot.amount <= EPS_D) {
      positions_.pop_front();
    }
  }

  return realised_pnl;
}
} // namespace vault