#pragma once

#include "types.hpp"
#include <string>
#include <vector>

namespace data_loading {

/**
 * @brief A simple CSV parser for loading LOB and trades data.
 *
 * This class parses CSV files containing limit order book (LOB) snapshots
 * and trade records, returning them as vectors of structured data.
 */
class CSVParser {
public:
  /**
   * @brief Default constructor.
   *
   * Uses the default LOB depth level (25).
   */
  CSVParser() = default;

  /**
   * @brief Constructs a CSVParser with a specific LOB depth level.
   *
   * @param lob_depth_level Maximum depth level to read from LOB CSV.
   */
  explicit CSVParser(const int lob_depth_level);

  /**
   * @brief Parses a LOB CSV file into structured LOBData.
   *
   * @param filename Path to the LOB CSV file.
   * @return std::vector<raw_data::LOBData> Vector of LOB data entries.
   */
  std::vector<raw_data::LOBData> parse_lob(const std::string &filename);

  /**
   * @brief Parses a trades CSV file into structured TradeData.
   *
   * @param filename Path to the trades CSV file.
   * @return std::vector<raw_data::TradeData> Vector of trade data entries.
   */
  std::vector<raw_data::TradeData> parse_trades(const std::string &filename);

private:
  /// Maximum depth level to read from LOB CSV files. Default is 25 as in given
  /// MD file.
  int lob_depth_level_{25};
};

} // namespace data_loading
