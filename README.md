# CMF Backtesting Engine

This repository contains a completed test assignment for admission to **CMF**.  
The task was to develop a custom backtesting engine for implementing, testing, and evaluating algorithmic trading strategies.  

---

## Project Structure

The project is divided into two main parts:  

- **Engine** — delivered as a static library  
- **Examples** — demonstrating the capabilities of the engine  

The library encapsulates many aspects of strategy testing, allowing the user to define:  
- their own **strategies**  
- custom **metrics** based on portfolio performance  

### Library Modules
```
data_loader:
args_parses.hpp - command-line argument parser
csv_parser.hpp - LOB and trades data parser
csv.h - external header-only CSV parsing library

execution:
backtesting_engine.hpp - core backtesting engine
market_engine.hpp - market simulator executing orders based on the current order book
orders.hpp - order execution logic (LimitFok, LimitIoc, Market)

metrics:
metric_abstract.hpp - base class for metrics
metrics_calculator.hpp - metric aggregator
predefined_metrics.hpp - predefined metrics (PnL, SharpeRatio, etc.)

vaults:
portfolio.hpp - trader’s portfolio abstraction
predefined_strategies.hpp - predefined strategies (currently only one implemented: replaying trades from file)
strategies.hpp - strategy abstraction
types.hpp - core data types
loggin.hpp - just a simple logger with one level of debug
```

---

## Building the Library

The simplest way to build and install the library is:

```bash
(optional) clone this repo
cd cmf_hft
mkdir build && cd build
cmake .. && make && sudo make install
```

The engine logs a large number of events, so if you don’t want to see them on your screen, you’ll need to use a slightly different command:

```bash
cmake .. -DCMAKE_CXX_FLAGS="-DNO_LOGGING" && make && sudo make install
```

the library will be available in `/usr/local/lib` and can be linked into your project.

## Examples
Several examples are provided to demonstrate the engine’s functionality:

`trade_from_files` — loads LOB and trades data, executing orders from the trades file  
`custom_strategy` — shows how to implement a custom strategy  
`custom_metric` — shows how to implement a custom metric

```bash
cd cmf_hft/examples
mkdir build && cd build
cmake .. && make
```

This will produce three executables in the build directory.

Run Examples  
`./trade_from_files --lob {path_to_lob_file}/lob.csv --trades {path_to_trades_file}/trades.csv`  
`./custom_metric   --lob {path_to_lob_file}/lob.csv --trades {path_to_trades_file}/trades.csv`  
`./custom_strategy --lob {path_to_lob_file}/lob.csv`  

For convenience, tiny versions of both `lob_tiny.csv` and `trades_tiny.csv` are included in the examples folder.

### 

---

## Usage Guide

### How to Write Your Own Strategies

The base `Strategy` class already provides access to several order book parameters.  
To create your own strategy, you need to **inherit** from this class and **override the required methods**:

```cpp
class StrategyBase {
public:
  using UPtr = std::unique_ptr<StrategyBase>;
  virtual ~StrategyBase() = default;
  virtual std::optional<common_types::Order> on_tick() = 0; // need to be implemented
  void set_current_data(const raw_data::LOBData &data) { current_data_ = data; }
protected:
  inline double mid_price() const { return (best_bid() + best_ask()) / 2.0; }
  double best_bid() const;
  double best_ask() const;
  common_types::Order create_buy_order(double amount, double price = .0) const;
  common_types::Order create_sell_order(double amount, double price = .0) const;
protected:
  raw_data::LOBData current_data_;
};
```

Example:

```cpp
class MyCustomStrategy : public vault::StrategyBase {
public:
    std::optional<Order> on_tick(const LOBData& data) override {
        if (data.best_ask - data.best_bid < 0.1) {
            return create_sell_order(10, 10);
        }
        return std::nullopt;
    }
};
```

## How to Write Your Own Metrics

Metrics are written in the same way as strategies:
inherit from the base Metric class  
implement the required calculation method  
register the metric in the MetricsCalculator  

```cpp
class MetricAbstract {
public:
  using UPtr = std::unique_ptr<MetricAbstract>;
  virtual ~MetricAbstract() = default;
  virtual double calculate(const Portfolio &portfolio) const = 0;  // need to be implemented
  inline virtual std::string getName() const { return name_; }
protected:
  std::string name_;
};

Example:
class MyMetric : public vault::stats::MetricAbstract {
public:
    MyMetric() { name_ = "TotalReturn"; }

    double calculate(const vault::Portfolio &portfolio) const override {
        return 42.0;
    }
};
```

Every custom metric must be registered in Metrics calculator:
```cpp
auto my_metric = std::make_unique<MyMetric>();
metrics_calculator.register_metric(std::move(my_metric));
```

The you can acure the result with:

```cpp
  const auto metrics = metrics_calculator.calculate_all_metrics(*portfolio);
  for (const auto &m : metrics) {
    std::cout << m.first << " : " << m.second << std::endl;
  }
  std::cout << std::endl;
```

### How to Run the Engine
The snippet below shows a minimal example of running a backtest:

```cpp
auto portfolio = vault::Portfolio::create_portfolio();
portfolio->set_amount(1000);           // start with 1000 assets
portfolio->set_cash(1000000);          // initial cash = 1M

auto strat = std::make_unique<vault::StrategyFromTradesFile>(trades_data);

vault::stats::MetricsCalculator metrics_calculator;
execution::BacktestEngine eng;

eng.link_portfolio(portfolio);         // user must connect portfolio to engine
eng.add_data(lob_data);                // load LOB data
eng.set_strategy(std::move(strat));    // user must assign chosen strategy
const auto res = eng.run();            // run backtest loop

if (!res)
{
  std::cout << "Run failure\n";
}

const auto metrics = metrics_calculator.calculate_all_metrics(*portfolio);

for (const auto &m : metrics) {
    std::cout << m.first << " : " << m.second << std::endl;
}
```

## Executing
After the running one of examples or you custom program with libhft you may see output like this:
```bash
[BACKTEST] Tick #22 ts=1722470413451824
[ENGINE] Adding order to pool: side=1 amount=9068.4 price=0.011025
[ENGINE] Strategy generated order: side=1 amount=9068.4 price=0.011025
[EXEC] MARKET SELL amount=9068.4
[EXEC][MARKET SELL] Fill: amount=9068.4 @ price=0.011025
[EXEC][MARKET SELL] Order fully executed.
[PORTFOLIO] Can sell? Need=9068.396936 Assets=10184.420027
[ENGINE] Portfolio CAN SELL. Executing...
[PORTFOLIO][PNL] common_types::Lot entry=0.011044 Sell=0.011025 Amount=84.645791 PnL=-0.001574
[PORTFOLIO][PNL] common_types::Lot entry=0.011029 Sell=0.011025 Amount=8983.751145 PnL=-0.033240
[PORTFOLIO][SELL] Sold amount=9068.396936 @ 0.011025 Cash now=10097.977383 Assets now=1116.023091 RealizedPnL=-0.034814
------------
pnl : -0.110926
max_drawdown : 0.00152778
sharpe_ratio : -0.290193
```

There is debug of action on every tick and result of registered metrics