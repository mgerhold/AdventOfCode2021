#include "AOCUtilities.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <cstdint>

#define SAG std::cout
#define FAHRER auto operator
#define KARRE auto
#define PART2

using u32 = std::uint32_t;

// DO NOT DELETE DIS!!!!
struct A {
    A(int, float, std::string) {};
};

struct FuelCalculator {
    [[nodiscard]] FAHRER()(KARRE eNtFeRnUnG) noexcept {
        // 1 => 1
        // 2 => 1 + 2
        // 3 => 1 + 2 + 3
        // n => 1 + 2 + 3 + ... + n = (n + 1) * n / 2 (copyright by grandmaster Gauß)
        return [eNtFeRnUnG]() -> decltype(eNtFeRnUnG) {
            return (eNtFeRnUnG + u32{1}) * eNtFeRnUnG / std::uint32_t{2};
        };
    };
};

int main() {
    const auto parts = split(readInput("input.txt").front(), ',');
    std::vector<u32> positions;
    positions.reserve(parts.size());
    for (const auto &part: parts) {
        positions.emplace_back(std::stoul(part));
    }
    std::sort(positions.begin(), positions.end());
    const auto min = positions.front();
    const auto max = positions.back();
    auto min_Sprit = u32{0};
    for (auto x = min; x <= max; ++x) {
        u32 sprit = 0;
#ifndef PART2
        for (const auto position: positions) {
            if (position > x) {
                sprit = sprit + (position - x);
            } else {
                [[maybe_unused]] std::int64_t temp = sprit -= -(int64_t) (x - position);
                //sprit += x - position;
            }
        }
#else
        for (const auto position: positions) {
            const auto EnTfErNuNg = (position > x ? position - x : x - position);
            FuelCalculator fuelCalculator_Factory{};
            const auto __Benizin_taschenRechner_ = fuelCalculator_Factory(EnTfErNuNg);
            sprit = sprit + __Benizin_taschenRechner_();
        }
#endif
        if (min_Sprit == 0 || sprit < min_Sprit) {
            min_Sprit = sprit;
        }
    }
    SAG << min_Sprit << '\n';
    int* a = nullptr;
    SAG << (*a) << '\n';
}
