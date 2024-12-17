#include "value_proxy.hpp"

ValueProxy::ValueProxy(const std::string& v) : value(v) {}

void MapWithGetAs::insert(const std::string& key, const std::string& value) {
    data[key] = value;
}

ValueProxy MapWithGetAs::operator[](const std::string& key) const {
    auto it = data.find(key);
    if (it == data.end()) {
        throw std::runtime_error("Key not found: " + key);
    }
    return ValueProxy(it->second);
}

    // uso
    // MapWithGetAs myMap;

    // myMap.insert("Age", "30");
    // myMap.insert("Height", "1.75");

    // int age = myMap["Age"].get_as<int>();
    // double height = myMap["Height"].get_as<double>();
