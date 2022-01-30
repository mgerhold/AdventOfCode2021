#include <algorithm>
#include <concepts>
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <cstdint>
#include <cassert>

struct DigitCounter {
    DigitCounter(std::size_t numDigits) {
        counts.resize(numDigits);
    }

    DigitCounter(const std::string_view numberString) {
        counts.resize(numberString.length());
        for (std::size_t i = 0; i < numberString.length(); ++i) {
            if (numberString[i] == '1') {
                ++counts[i];
            } else if (numberString[i] != '0') {
                assert(false);
            }
        }
    }

    DigitCounter(std::vector<std::uint32_t> counts) : counts{ std::move(counts) } { }

    [[nodiscard]] explicit operator std::uint32_t() const {
        auto result = std::uint32_t{ 0 };
        for (std::size_t i = 0; i < counts.size(); ++i) {
            const auto index = counts.size() - 1 - i;
            result += (counts[index] << i);
        }
        return result;
    }

    DigitCounter operator+=(const DigitCounter& other) {
        assert(counts.size() == other.counts.size());
        auto oldCounts = counts;
        for (std::size_t i = 0; i < counts.size(); ++i) {
            counts[i] += other.counts[i];
        }
        return DigitCounter{ std::move(oldCounts) };
    }

    [[nodiscard]] DigitCounter operator+(const DigitCounter& other) const {
        return DigitCounter{ counts } += other;
    }

    [[nodiscard]] DigitCounter operator~() const {
        auto resultCounts = counts;
        for (auto& count : resultCounts) {
            count = !count;
        }
        return resultCounts;
    }

    std::vector<std::uint32_t> counts;
};

std::ostream& operator<<(std::ostream& ostream, const DigitCounter& digitCounter) {
    if (digitCounter.counts.size() == 0) {
        return ostream;
    }
    ostream << "(" << digitCounter.counts.front();
    for (std::size_t i = 1; i < digitCounter.counts.size(); ++i) {
        ostream << "," << digitCounter.counts[i];
    }
    ostream << ")";
    return ostream;
}

[[nodiscard]] auto readInput(const std::string& filename) {
    std::vector<std::string> result;
    std::ifstream inputStream{ filename };
    while (inputStream.good()) {
        std::string line;
        std::getline(inputStream, line);
        result.emplace_back(std::move(line));
    }
    return result;
}

void part1() {
    const auto input = readInput("input.txt");
    /*const auto input = std::vector<std::string>{
        "00100",
        "11110",
        "10110",
        "10111",
        "10101",
        "01111",
        "00111",
        "11100",
        "10000",
        "11001",
        "00010",
        "01010",
    };*/
    DigitCounter accumulator{ input.front().length() };
    for (const auto& numberString : input) {
        DigitCounter counter{ numberString };
        accumulator += counter;
    }
    DigitCounter gammaCounter{input.front().length() };
    for (std::size_t i = 0; i < gammaCounter.counts.size(); ++i) {
        gammaCounter.counts[i] = (accumulator.counts[i] > input.size() / 2 ? 1 : 0);
    }
    const auto gamma = static_cast<std::uint32_t>(gammaCounter);
    const auto epsilon = static_cast<std::uint32_t>(~gammaCounter);
    const auto powerConsumption = gamma * epsilon;
    std::cout << gammaCounter << "\n";
    std::cout << gamma << "\n";
    std::cout << epsilon << "\n";
    std::cout << powerConsumption << "\n";
}

[[nodiscard]] auto getResult(std::vector<DigitCounter> input,
                             std::invocable<std::uint32_t, std::uint32_t> auto&& predicate = std::equal_to{}) {
    const auto numDigits = input.front().counts.size();
    for (std::size_t i = 0; i < numDigits; ++i) {
        const auto numberOfOnes = std::count_if(input.begin(), input.end(), [i](const auto& digitCounter) {
            return digitCounter.counts[i] == 1;
        });
        const auto mostCommon = (numberOfOnes >= input.size() / 2 + (input.size() & 1) ? 1 : 0);
        input.erase(std::remove_if(input.begin(),
                                       input.end(),
                                       [i, mostCommon, &predicate](const auto& digitCounter) {
                                           return predicate(digitCounter.counts[i], mostCommon);
                                       }),
                    input.end());
        if (input.size() == 1) {
            return static_cast<std::uint32_t>(input.front());
        }
    }
    assert(false);
    return std::uint32_t{ 0 };
}

void part2() {
    const auto input = readInput("input.txt");
    /*const auto input = std::vector<std::string>{
        "00100",
        "11110",
        "10110",
        "10111",
        "10101",
        "01111",
        "00111",
        "11100",
        "10000",
        "11001",
        "00010",
        "01010",
    };*/
    std::vector<DigitCounter> inputCounters;
    inputCounters.reserve(input.size());
    for (const auto& numberString : input) {
        DigitCounter counter{ numberString };
        inputCounters.emplace_back(counter);
    }
    const auto oxygenRating = getResult(inputCounters, std::not_equal_to{});
    const auto co2scrubberRating = getResult(inputCounters, std::equal_to{});
    const auto lifeSupportRating = oxygenRating * co2scrubberRating;
    std::cout << "Oxygen Rating: " << oxygenRating << "\n";
    std::cout << "CO2 Scrubber Rating: " << co2scrubberRating << "\n";
    std::cout << "Life Support Rating: " << lifeSupportRating << "\n";
}

int main() {
    // part1();
    part2();
}
