#include "AOCUtilities.hpp"
#include <array>
#include <iostream>
#include <numeric>
#include <string_view>
#include <unordered_map>
#include <queue>
#include <cassert>

struct Insertion {
    char character;
    uz position;

    [[nodiscard]] bool operator>(const Insertion& other) const {
        return position > other.position;
    }
};

struct CharPair {
    std::array<char, 2> chars;

    [[nodiscard]] bool operator==(const CharPair&) const = default;
};

namespace std {
    template<>
    struct hash<CharPair> {
        uz operator()(const CharPair& charPair) const {
            return std::hash<uz>{}((static_cast<uz>(charPair.chars[0]) << sizeof(charPair.chars[0])) +
                                   static_cast<uz>(charPair.chars[1]));
        }
    };
}// namespace std

void printResults(const std::unordered_map<char, uz>& counts) {
    const auto max = std::max_element(counts.begin(), counts.end(),
                                      [](const auto& lhs, const auto& rhs) { return lhs.second < rhs.second; });
    assert(max != counts.end());
    const auto min = std::min_element(counts.begin(), counts.end(),
                                      [](const auto& lhs, const auto& rhs) { return lhs.second < rhs.second; });
    assert(min != counts.end());
    std::cout << "Most common element is " << max->first << " with " << max->second << " occurrences\n";
    std::cout << "Least common element is " << min->first << " with " << min->second << " occurrences\n";
    std::cout << "Difference: " << (max->second - min->second) << "\n";
}

template<typename Map, typename Key, typename Value>
void addOrCreate(Map& map, Key key, Value value) {
    map[key] = (map.contains(key) ? map[key] + value : value);
}

void part1(std::string polymerTemplate, std::unordered_map<CharPair, char> pairInsertionRules) {
    constexpr auto numSteps = 10;
    for (int step = 0; step < numSteps; ++step) {
        std::priority_queue<Insertion, std::vector<Insertion>, std::greater<>> insertions;
        for (auto i = uz{ 0 }; i < polymerTemplate.length(); ++i) {
            const auto findIterator = pairInsertionRules.find({ polymerTemplate[i], polymerTemplate[i + 1] });
            if (findIterator != pairInsertionRules.end()) {
                insertions.emplace(findIterator->second, i + 1);
            }
        }

        auto offset = uz{ 0 };
        while (!insertions.empty()) {
            const auto& insertion = insertions.top();
            polymerTemplate.insert(insertion.position + offset, 1, insertion.character);
            ++offset;
            insertions.pop();
        }
    }

    std::unordered_map<char, uz> counts;
    for (const auto c : polymerTemplate) {
        addOrCreate(counts, c, 1);
    }
    printResults(counts);
}

[[nodiscard]] auto pairCountsFromString(const std::string& string) {
    std::unordered_map<CharPair, uz> pairCounts;
    for (auto i = uz{ 0 }; i < string.length() - 1; ++i) {
        addOrCreate(pairCounts, CharPair{ string.at(i), string.at(i + 1) }, 1);
    }
    return pairCounts;
}

void part2(const std::string& polymerTemplate, const std::unordered_map<CharPair, char>& pairInsertionRules) {
    auto pairCounts = pairCountsFromString(polymerTemplate);
    constexpr auto numSteps = 40;
    std::unordered_map<CharPair, uz> newPairInsertions;
    newPairInsertions.reserve(pairInsertionRules.size());
    for (auto i = 0; i < numSteps; ++i) {
        newPairInsertions.clear();
        for (const auto& insertionRule : pairInsertionRules) {
            const auto findIterator = pairCounts.find(insertionRule.first);
            if (findIterator == pairCounts.end()) {
                continue;
            }
            const auto newPairs = std::array{ CharPair{ findIterator->first.chars[0], insertionRule.second },
                                              CharPair{ insertionRule.second, findIterator->first.chars[1] } };
            for (const auto& pair : newPairs) {
                addOrCreate(newPairInsertions, pair, findIterator->second);
            }
            pairCounts.erase(findIterator);
        }
        for (const auto& insertion : newPairInsertions) {
            addOrCreate(pairCounts, insertion.first, insertion.second);
        }
    }
    auto counts = std::unordered_map<char, uz>{};
    for (const auto& pairCount : pairCounts) {
        for (const auto c : pairCount.first.chars) {
            addOrCreate(counts, c, pairCount.second);
        }
    }
    std::for_each(counts.begin(), counts.end(), [](auto& count) { count.second = (count.second + 1) / 2; });
    printResults(counts);
}

int main() {
    const auto lines = readInput("input.txt");
    const auto& polymerTemplate = lines.front();
    std::unordered_map<CharPair, char> pairInsertionRules;
    for (auto i = uz{ 2 }; i < lines.size(); ++i) {
        pairInsertionRules[{ lines[i][0], lines[i][1] }] = lines[i].at(6);
    }
    //part1(polymerTemplate, pairInsertionRules);
    part2(polymerTemplate, pairInsertionRules);
}
