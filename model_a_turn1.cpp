#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <shared_mutex>
#include <algorithm>

struct Order {
    int lotSize;
    int price;
    
    Order() : lotSize(10), price(2) {}
    
    Order(int lotSize, int price) : lotSize(lotSize), price(price) {}
    
    bool operator==(const Order& other) const {
        return price == other.price;
    }
};

class ConcurrentHashMap {
public:
    void insert(const std::string& symbol, const Order& order) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (map_.find(symbol) == map_.end()) {
            map_[symbol] = std::vector<Order>();
        }
        map_[symbol].push_back(order);
    }

    void update(const std::string& symbol, int price, const Order& newOrder) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        auto it = map_.find(symbol);
        if (it != map_.end()) {
            auto& orders = it->second;
            auto orderIt = std::find_if(orders.begin(), orders.end(), [price](const Order& order) {
                return order.price == price;
            });
            if (orderIt != orders.end()) {
                *orderIt = newOrder;
            }
        }
    }

    void remove(const std::string& symbol) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        map_.erase(symbol);
    }

    void display() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        for (const auto& pair : map_) {
            std::cout << "Symbol: " << pair.first << "\n";
            for (const auto& order : pair.second) {
                std::cout << "  Price: " << order.price << ", LotSize: " << order.lotSize << "\n";
            }
        }
    }

private:
    std::map<std::string, std::vector<Order>> map_;
    std::shared_mutex mutex_;
};

int main() {
    ConcurrentHashMap concurrentMap;

    // Inserting new data
    concurrentMap.insert("NESTLEIND", Order(15, 150));
    concurrentMap.insert("HDFCBANK", Order(20, 1400));
    concurrentMap.insert("RELIANCE", Order(25, 2500));

    // Updating order
    concurrentMap.update("NESTLEIND", 150, Order(18, 155));

    // Removing symbol
    concurrentMap.remove("HDFCBANK");

    // Display the map
    concurrentMap.display();

    return 0;
}
