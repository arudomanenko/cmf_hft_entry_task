#pragma once
#include "vaults/strategies.hpp"

namespace vault {

/**
 * @brief Strategy that replays trades from a given trades file.
 *
 * Generates orders on each tick according to the timestamps in the trade data.
 * Useful for backtesting historical trade sequences.
 */
class StrategyFromTradesFile : public StrategyBase {
public:
  /**
   * @brief Constructs the strategy from a vector of trade data.
   *
   * @param trades Vector of trades to replay. Each trade will generate an
   * order.
   */
  StrategyFromTradesFile(const std::vector<raw_data::TradeData> &trades);

  /**
   * @brief Processes the next tick and possibly generates an order.
   *
   * Compares the current LOB tick timestamp (current_data_.local_timestamp)
   * with the timestamp of the next trade in the trades vector. If the tick is
   * at or after the trade timestamp, returns a Limit IOC order matching that
   * trade.
   *
   * @return std::optional<common_types::Order> Returns the next order if ready,
   *         or std::nullopt if no order should be executed on this tick.
   */
  std::optional<common_types::Order> on_tick() override;

private:
  std::vector<raw_data::TradeData> trades_; ///< All trades to be replayed.
  std::vector<raw_data::TradeData>::const_iterator
      trade_it_; ///< Iterator to the next trade to execute.
};

} // namespace vault
