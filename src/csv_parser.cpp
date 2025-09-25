#include "data_loader/csv_parser.hpp"
#include "data_loader/csv.h"
#include "logging.hpp"
#include "types.hpp"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace {
std::unordered_map<std::string, common_types::Side> PARSED_ACTION_TO_ENUM_TYPE{
    {"sell", common_types::Side::Sell}, {"buy", common_types::Side::Buy}};

void to_lower(std::string &src) {
  std::transform(src.begin(), src.end(), src.begin(),
                 [](unsigned char c) { return std::tolower(c); });
}
} // namespace

namespace data_loading {
CSVParser::CSVParser(const int lob_depth_level)
    : lob_depth_level_(lob_depth_level) {}

std::vector<raw_data::LOBData>
CSVParser::parse_lob(const std::string &filename) {
  std::vector<raw_data::LOBData> lob_data;

  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  std::string line;
  std::getline(file, line);

  int line_count = 0;
  while (std::getline(file, line)) {
    line_count++;
    if (line_count % 100000 == 0) {
      logging::Logger::debug("on_ticked ", line_count, " LOB lines...");
    }

    raw_data::LOBData entry;
    std::stringstream ss(line);
    std::string cell;

    std::getline(ss, cell, ',');
    std::getline(ss, cell, ',');
    try {
      entry.local_timestamp = std::stoll(cell);
    } catch (const std::exception &e) {
      logging::Logger::debug("Error parsing timestamp at line ", line_count,
                             ": ", cell);
      continue;
    }

    for (int i = 0; i < lob_depth_level_; ++i) {
      double ask_price = 0.0, ask_amount = 0.0, bid_price = 0.0,
             bid_amount = 0.0;

      try {
        if (!std::getline(ss, cell, ','))
          break;
        ask_price = std::stod(cell);

        if (!std::getline(ss, cell, ','))
          break;
        ask_amount = std::stod(cell);

        if (!std::getline(ss, cell, ','))
          break;
        bid_price = std::stod(cell);

        if (!std::getline(ss, cell, ','))
          break;
        bid_amount = std::stod(cell);

      } catch (const std::exception &e) {
        logging::Logger::debug("Error parsing level ", i, " at line ",
                               line_count, ": ", e.what());
        continue;
      }

      if (ask_price > 0 && ask_amount > 0) {
        entry.asks.push_back({ask_price, ask_amount});
      }
      if (bid_price > 0 && bid_amount > 0) {
        entry.bids.push_back({bid_price, bid_amount});
      }
    }

    lob_data.push_back(entry);
  }

  logging::Logger::debug("Total LOB entries loaded: ", lob_data.size());
  return lob_data;
}

std::vector<raw_data::TradeData>
CSVParser::parse_trades(const std::string &filename) {
  std::vector<raw_data::TradeData> trades;

  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  std::string line;
  std::getline(file, line);

  int line_count = 0;
  while (std::getline(file, line)) {
    line_count++;
    if (line_count % 100000 == 0) {
      logging::Logger::debug("on_ticked ", line_count, " trade lines...");
    }

    raw_data::TradeData trade;
    std::stringstream ss(line);
    std::string cell;

    std::getline(ss, cell, ',');

    std::getline(ss, cell, ',');
    try {
      trade.local_timestamp = std::stoll(cell);
    } catch (const std::exception &e) {
      logging::Logger::debug("Error parsing timestamp at line ", line_count,
                             ": ", cell);
      continue;
    }

    std::getline(ss, cell, ',');

    to_lower(cell);
    trade.side = PARSED_ACTION_TO_ENUM_TYPE[cell];

    std::getline(ss, cell, ',');
    try {
      trade.price = std::stod(cell);
    } catch (const std::exception &e) {
      logging::Logger::debug("Error parsing price at line ", line_count, ": ",
                             cell);
      continue;
    }

    std::getline(ss, cell, ',');
    try {
      trade.amount = std::stod(cell);
    } catch (const std::exception &e) {
      logging::Logger::debug("Error parsing amount at line ", line_count, ":",
                             cell);
      continue;
    }

    trades.push_back(trade);
  }

  logging::Logger::debug("Total trades loaded: ", trades.size());
  return trades;
}
} // namespace data_loading