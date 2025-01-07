#pragma once

#include <string>
#include <sstream>
#include <stdexcept>
#include <map>
#include <string>
#include <stdexcept>

class ValueProxy {
private:
    const std::string& value;

public:
    ValueProxy(const std::string& v);

    template <typename T>
    T get_as() const {
        std::istringstream iss;
        T result;
        if (!(iss >> result)) {
            throw std::runtime_error("Failed to convert value to requested type.");
        }
        return result;
    }
};

template <typename T,typename K>
class MapWithGetAs {
private:
    std::map<T, K> data;

public:
    void insert(const T& key, K& value);
    ValueProxy operator[](const T& key) const;
};


