#pragma once

#include <unordered_map>

using vec3 = float;
using Key = std::pair<std::string, std::string>;

template <>
struct std::hash<Key>
{
    std::size_t operator()(const Key& pair) const
    {
        std::size_t h1 = std::hash<std::string>()(pair.first);
        std::size_t h2 = std::hash<std::string>()(pair.second);

        return h1 ^ h2;
    }
};

class Result
{
public:
    std::unordered_map<Key, vec3> values;
};
