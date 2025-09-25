#pragma once

#include <iostream>
#include <string>

/**
 * @brief Structure to store program command-line arguments.
 */
struct ProgramArgs {
  /// Path to the LOB csv file (mandatory).
  std::string lob;
  /// Path to the trades csv file (optional).
  std::string trades;
};

/**
 * @brief Prints the usage instructions for the program.
 *
 * @param program_name Name of the executable (usually argv[0]).
 */
inline void print_usage(const std::string &program_name) {
  std::cout << "Usage: " << program_name << " --lob <value> [--trades <value>]"
            << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  --lob <value>     Specify path to lob.csv (mandatory)"
            << std::endl;
  std::cout << "  --trades <value>  Specify path to trades.csv (optional)"
            << std::endl;
  std::cout << "  --help            Show this help message" << std::endl;
}

/**
 * @brief Parses command-line arguments into a ProgramArgs structure.
 *
 * Supported arguments:
 * - --lob <value>      : Path to the LOB CSV file (mandatory).
 * - --trades <value>   : Path to the trades CSV file (optional).
 * - --help             : Prints usage instructions and exits.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return ProgramArgs Structure containing parsed paths.
 *
 * @throws std::runtime_error If --lob is missing or unknown arguments are
 * provided.
 */
inline ProgramArgs parse_arguments(int argc, char *argv[]) {
  ProgramArgs args;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "--lob") {
      if (i + 1 < argc) {
        args.lob = argv[++i];
      } else {
        throw std::runtime_error("--lob requires a value");
      }
    } else if (arg == "--trades") {
      if (i + 1 < argc) {
        args.trades = argv[++i];
      } else {
        throw std::runtime_error("--trades requires a value");
      }
    } else if (arg == "--help") {
      print_usage(argv[0]);
      exit(0);
    } else {
      throw std::runtime_error("Unknown argument: " + arg);
    }
  }

  if (args.lob.empty()) {
    throw std::runtime_error("--lob parameter is required");
  }

  return args;
}
