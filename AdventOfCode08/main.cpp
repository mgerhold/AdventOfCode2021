#include "AOCUtilities.hpp"
#include <iostream>
#include <unordered_map>
#include <cassert>

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using uz = std::size_t;

void part1() {
    const auto mappings = std::unordered_map<uz, u8>{
            { 2, 1 },
            { 3, 7 },
            { 4, 4 },
            { 7, 8 }
    };
    const auto lines = readInput("input.txt");
    auto accumulator = u32{ 0 };
    for (const auto& line : lines) {
        const auto parts = split(line, '|');
        const auto outputs = split(parts.back(), ' ');
        for (const auto& output : outputs) {
            const auto trimmed = trim(output, ' ');
            accumulator += static_cast<u32>(mappings.contains(trimmed.length()));
        }
    }
    std::cout << accumulator << '\n';
}

[[nodiscard]] std::string popByLength(std::vector<std::string>& strings, uz length) {
    const auto findIterator = std::find_if(strings.begin(), strings.end(), [length](const auto& string) {
        return string.length() == length;
    });
    assert(findIterator != strings.end());
    auto result = std::move(*findIterator);
    strings.erase(findIterator);
    return result;
}

[[nodiscard]] std::string difference(std::string minuend, const std::string& subtrahend) {
    for (const auto toErase : subtrahend) {
        minuend.erase(std::remove(minuend.begin(), minuend.end(), toErase), minuend.end());
    }
    return minuend;
}

[[nodiscard]] std::string findKey(const std::unordered_map<std::string, u8>& mappings, const u8 value) {
    const auto it = std::find_if(mappings.begin(), mappings.end(), [value](const auto& pair) {
        return pair.second == value;
    });
    return it->first;
}

[[nodiscard]] bool isSubsetOf(const std::string& subset, const std::string& superset) {
    for (const auto c : subset) {
        if (superset.find(c) == std::string::npos) {
            return false;
        }
    }
    return true;
}

void part2() {
    const auto lines = readInput("input.txt");
    auto accumulator = u32{ 0 };
    for (const auto& line : lines) {
        std::unordered_map<std::string, u8> mappings;
        const auto parts = split(line, '|');
        auto digitStrings = split(parts.front(), ' ');
        for (auto& digitString : digitStrings) {
            digitString = trim(digitString, ' ');
            std::sort(digitString.begin(), digitString.end());
        }
        mappings[popByLength(digitStrings, 2)] = 1;
        mappings[popByLength(digitStrings, 3)] = 7;
        mappings[popByLength(digitStrings, 4)] = 4;
        mappings[popByLength(digitStrings, 7)] = 8;
        const auto a = difference(findKey(mappings, 7), findKey(mappings, 1));

        // NINE = genau wie 4 + a und einem weiteren (nämlich g)
        auto nineStringHelper = findKey(mappings, 4) + a; // still missing the letter that maps to g
        const auto nineIterator = std::find_if(digitStrings.begin(), digitStrings.end(),
                                               [&nineStringHelper](const auto& string) {
            if (string.length() != nineStringHelper.length() + 1) {
                return false;
            }
            for (const auto c : nineStringHelper) {
                if (string.find(c) == std::string::npos) {
                    return false;
                }
            }
            return true;
        });
        auto nineString = *nineIterator;
        digitStrings.erase(nineIterator);
        std::sort(nineString.begin(), nineString.end());
        mappings[nineString] = 9;
        const auto e = difference(findKey(mappings, 8), findKey(mappings, 9));

        // TWO = einzige fünfstellige Kombination mit e
        const auto twoIterator = std::find_if(digitStrings.begin(), digitStrings.end(), [&e](const auto& string) {
            return string.length() == 5 && string.find(e) != std::string::npos;
        });
        assert(twoIterator != digitStrings.end());
        mappings[*twoIterator] = 2;
        digitStrings.erase(twoIterator);

        // ZERO = sechsstellige Zahl, die nicht NINE ist und ONE enthält
        const auto zeroIterator = std::find_if(digitStrings.begin(), digitStrings.end(),
                                               [nine = findKey(mappings, 9), one = findKey(mappings, 1)](const auto& string) {
            return string.length() == 6 && string != nine && isSubsetOf(one, string);
        });
        assert(zeroIterator != digitStrings.end());
        mappings[*zeroIterator] = 0;
        digitStrings.erase(zeroIterator);

        // SIX = sechsstellige Zahl, die nicht ZERO ist und nicht NINE ist
        const auto sixIterator = std::find_if(digitStrings.begin(), digitStrings.end(),
                                              [zero = findKey(mappings, 0), nine = findKey(mappings, 9)](const auto& string) {
            return string.length() == 6 && string != zero && string != nine;
        });
        assert(sixIterator != digitStrings.end());
        mappings[*sixIterator] = 6;
        digitStrings.erase(sixIterator);

        // THREE = fünfstellige Zahl, die ONE enthält
        const auto threeIterator = std::find_if(digitStrings.begin(), digitStrings.end(),
                                                [one = findKey(mappings, 1)](const auto& string) {
            return string.length() == 5 && isSubsetOf(one, string);
        });
        assert(threeIterator != digitStrings.end());
        mappings[*threeIterator] = 3;
        digitStrings.erase(threeIterator);
        for (const auto& pair : mappings) {
            std::cout << static_cast<u32>(pair.second) << " == " << pair.first << "\n";
        }
        assert(digitStrings.size() == 1);
        // FIVE = fünfstellige Zahl, die nicht THREE ist und nicht TWO ist
        // (easier: last remaining digit)
        mappings[digitStrings.front()] = 5;
        assert(mappings.size() == 10);

        const auto outputs = split(parts.back(), ' ');
        assert(outputs.size() == 4);
        auto decoded = u32{ 0 };
        u32 factor = 1000;
        for (uz i = 0; i < outputs.size(); ++i, factor /= 10) {
            auto trimmed = trim(outputs[i], ' ');
            std::sort(trimmed.begin(), trimmed.end());
            decoded += mappings[trimmed] * factor;
        }
        accumulator += decoded;
    }
    std::cout << "Accumulated result: " << accumulator << "\n";
}

int main() {
    // part1();
    part2();
}
