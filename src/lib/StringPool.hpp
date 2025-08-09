#pragma once

#include <vector>
#include <unordered_map>
#include <string_view>
#include <string>
#include <inttypes.h>
#include <Architecture.hpp>

class StringPool
{
public:
    uint64_t intern(const std::string& str)
    {
        auto it = map.find(str);
        if (it != map.end()) return it->second;

        uint64_t id = static_cast<uint64_t>(storage.size());
        storage.push_back(str);
        map[str] = id;
        return id;
    }

    const std::string& lookup(uint64_t id) const
    {
        return storage[id];
    }

private:
    std::unordered_map<std::string, uint64_t> map;
    std::vector<std::string> storage;
};