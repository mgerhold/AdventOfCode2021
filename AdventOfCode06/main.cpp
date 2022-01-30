#include "AOCUtilities.hpp"
#include <array>
#include <numeric>
#include <iostream>
#include <vector>
#include <cstdint>

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using uz = std::size_t;
using Fish = u8;
using Days = u32;

[[nodiscard]] std::size_t populationAfterTime(const std::vector<Fish>& startingPopulation,
                                              const Days days) {
    std::array<u64, 9> fishWithCounters{};
    for (const auto fish : startingPopulation) {
        ++fishWithCounters.at(fish);
    }
    for (Days day = 0; day < days; ++day) {
        u64 fishToSpawn = fishWithCounters.at(0);
        for (uz counter = 0; counter < fishWithCounters.size() - 1; ++counter) {
            fishWithCounters.at(counter) = fishWithCounters.at(counter + 1);
        }
        fishWithCounters.at(6) += fishToSpawn;
        fishWithCounters.at(8) = fishToSpawn;
    }
    return std::accumulate(begin(fishWithCounters), end(fishWithCounters), u64{ 0 });
}

int main() {
    const auto inputParts = split(readInput("input.txt").front(), ',');
    std::vector<Fish> population;
    population.reserve(inputParts.size());
    for (const auto& part : inputParts) {
        population.emplace_back(static_cast<Fish>(std::stoi(part)));
    }
    std::cout << "After 18 days: " << populationAfterTime(population, 18) << "\n";
    std::cout << "After 80 days: " << populationAfterTime(population, 80) << "\n";
    // part 2:
    std::cout << "After 256 days: " << populationAfterTime(population, 256) << "\n";
}
