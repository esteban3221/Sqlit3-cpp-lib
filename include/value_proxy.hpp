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
        std::istringstream iss(value);
        T result;
        if (!(iss >> result)) {
            throw std::runtime_error("Failed to convert value to requested type.");
        }
        return result;
    }
};

class MapWithGetAs {
private:
    std::map<std::string, std::string> data;

public:
    void insert(const std::string& key, const std::string& value);

    ValueProxy operator[](const std::string& key) const;
};


