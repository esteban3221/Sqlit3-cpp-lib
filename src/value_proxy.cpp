#include "value_proxy.hpp"

ValueProxy::ValueProxy(const std::string &v) : value(v) {}

template <typename T, typename K>
void MapWithGetAs<T, K>::insert(const T &key, K &value)
{
    data[key] = value;
}

template <typename T, typename K>
ValueProxy MapWithGetAs<T, K>::operator[](const T &key) const
{
    auto it = data.find(key);
    if (it == data.end())
    {
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
