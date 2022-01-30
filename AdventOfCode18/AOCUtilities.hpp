//
// Created by coder2k on 06.12.2021.
//

#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <cstdint>

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using uz = std::size_t;

[[nodiscard]] inline auto readInput(const std::string& filename) {
    const auto path = std::filesystem::path{ filename };
    if (!exists(path)) {
        throw std::runtime_error{ "The specified file does not exist. " };
    }
    std::vector<std::string> result;
    std::ifstream inputStream{ filename };
    if (!inputStream.good()) {
        throw std::runtime_error{ "Unable to read file (maybe empty?). " };
    }
    while (inputStream.good()) {
        std::string line;
        std::getline(inputStream, line);
        result.emplace_back(std::move(line));
    }
    return result;
}

[[nodiscard]] inline std::vector<std::string> split(const std::string& s, const char delimiter = ' ') {
    std::string current;
    std::vector<std::string> result;
    for (char c : s) {
        if (c == delimiter && !current.empty()) {
            result.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        result.push_back(current);
    }
    return result;
}

[[nodiscard]] inline std::string trim(const std::string& string, const char charToTrim = ' ') {
    /* the bounds calculated describe the characters that should NOT be deleted, that's
     * why they are exclusive in the context of the characters that SHOULD be deleted */
    std::size_t leftBoundExclusive = 0;
    std::size_t rightBoundExclusive = string.length() + 1;
    for (std::size_t i = 0; i < string.length(); ++i) {
        if (string[i] != charToTrim) {
            leftBoundExclusive = i;
            break;
        }
    }
    auto index = string.length() - 1;
    for (std::size_t i = 0; i < string.length(); ++i, --index) {
        if (string[index] != charToTrim) {
            rightBoundExclusive = index;
            break;
        }
    }
    // "   hallo  "
    //  0123456789
    // leftBoundExclusive = 3
    // rightBoundExclusive = 7
    const auto length = rightBoundExclusive - leftBoundExclusive + 1;
    return string.substr(leftBoundExclusive, length);
}

template<typename T>
[[nodiscard]] T median(std::vector<T>& values) {
    assert(!values.empty());
    if (values.size() == 1) {
        return values.front();
    }
    if (values.size() % 2 == 1) {
        std::nth_element(begin(values), begin(values) + static_cast<std::int64_t>(values.size() / 2), end(values));
        return values.at(values.size() / 2);
    }
    const auto middleIndex = values.size() / 2;
    std::partial_sort(begin(values), begin(values) + static_cast<std::int64_t>(middleIndex), end(values));
    return (values.at(middleIndex) + values.at(middleIndex - 1)) / 2;
}

template<typename T>
struct Point {
    T x, y;

    [[nodiscard]] bool operator==(const Point&) const = default;

    [[nodiscard]] Point operator+(const Point& other) const {
        auto result = *this;
        result += other;
        return result;
    }

    Point operator+=(const Point& other) {
        Point result{ x, y };
        x += other.x;
        y += other.y;
        return result;
    }
};

template<typename First, typename Second, typename... Remaining>
[[nodiscard]] inline uz combineHashes(const First first, const Second second, const Remaining... remaining) {
    if constexpr (sizeof...(remaining) == 0) {
        const auto firstHash = std::hash<First>{}(first);
        const auto secondHash = std::hash<Second>{}(second);
        return secondHash + 0x9e3779b9 + (firstHash << 6) + (firstHash >> 2);
    } else {
        return combineHashes(combineHashes(first, second), remaining...);
    }
}

using PointUZ = Point<uz>;
using PointU32 = Point<u32>;

namespace std {
    template<>
    struct hash<PointU32> {
        uz operator()(const PointU32& point) const {
            static_assert(sizeof(uz) == 8);
            return hash<uz>{}((static_cast<uz>(point.x) << 32) + static_cast<uz>(point.y));
        }
    };

    /*template<>
    struct hash<PointU32> {
        uz operator()(const PointU32 &point) const {
            std::hash<uz> hasher;
            const auto seed = hasher(point.x);
            return hasher(point.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };*/
}// namespace std