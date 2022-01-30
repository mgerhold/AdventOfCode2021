#include "AOCUtilities.hpp"
#include <iostream>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <utility>
#include <cmath>


void printMeasurements(const std::vector<std::vector<Point3D<i64>>>& measurements) {
    for (auto i = uz{}; i < measurements.size(); ++i) {
        std::cout << "Scanner " << i << "\n";
        for (const auto& point : measurements.at(i)) {
            std::cout << "\t(" << point.x << "," << point.y << "," << point.z << ")\n";
        }
    }
}

[[nodiscard]] i64 manhattanDistance(const Point3D<i64>& lhs, const Point3D<i64>& rhs) {
    return std::abs(lhs.x - rhs.x) + std::abs(lhs.y - rhs.y) + std::abs(lhs.z - rhs.z);
}

namespace std {
    template<>
    struct hash<std::pair<i64, i64>> {
        uz operator()(const std::pair<i64, i64>& pair) const {
            return combineHashes(pair.first, pair.second);
        }
    };
}// namespace std

int main() {
    auto measurements = std::vector<std::vector<Point3D<i64>>>{};
    const auto input = readInput("testcase.txt");
    using namespace std::string_view_literals;
    constexpr auto scannerPrefix = "--- scanner "sv;
    auto numInputBeacons = uz{};
    for (const auto& line : input) {
        if (line.starts_with(scannerPrefix)) {
            measurements.emplace_back();
            continue;
        }
        if (line.empty()) {
            continue;
        }
        const auto parts = split(line, ',');
        measurements.back().emplace_back(std::stoll(parts.at(0)), std::stoll(parts.at(1)), std::stoll(parts.at(2)));
        ++numInputBeacons;
    }
    // printMeasurements(measurements);
    auto distances = std::vector<std::vector<std::vector<i64>>>{};
    for (const auto& scanner : measurements) {
        distances.emplace_back();
        for (const auto& beacon : scanner) {
            distances.back().emplace_back();
            for (const auto& other : scanner) {
                const auto distance = manhattanDistance(beacon, other);
                distances.back().back().emplace_back(distance);
            }
        }
    }

    auto overlappingScanners = std::unordered_map<std::pair<i64, i64>, uz>{};
    for (auto i = uz{}; i < distances.size(); ++i) {
        const auto& scanner = distances.at(i);
        for (const auto& beacon : scanner) {
            for (auto j = i + 1; j < distances.size(); ++j) {
                const auto& otherScanner = distances.at(j);
                for (const auto& otherBeacon : otherScanner) {
                    auto counts = std::array{
                        std::unordered_map<i64, uz>{},
                        std::unordered_map<i64, uz>{},
                    };
                    for (const auto distance : beacon) {
                        counts[0][distance] = counts[0].contains(distance) ? counts[0][distance] + 1 : 1;
                    }
                    for (const auto distance : otherBeacon) {
                        counts[1][distance] = counts[1].contains(distance) ? counts[1][distance] + 1 : 1;
                    }
                    auto overlapping = uz{};
                    for (const auto& distanceCount : counts[0]) {
                        if (counts[1].contains(distanceCount.first)) {
                            overlapping += std::min(distanceCount.second, counts[1][distanceCount.first]);
                        }
                    }
                    if (overlapping >= 12) {
                        const auto pair = std::pair{ i, j };
                        overlappingScanners[pair] = overlappingScanners.contains(pair)
                                                            ? std::min(overlappingScanners[pair], overlapping)
                                                            : overlapping;
                        /*std::cout << "Scanners " << i << " and " << j << " are overlapping (" << overlapping
                                  << " common beacons)\n";*/
                    }
                }
            }
        }
    }
    auto numBeacons = numInputBeacons;
    for (const auto& overlappingScanner : overlappingScanners) {
        numBeacons -= overlappingScanner.second;
        std::cout << "Scanners " << overlappingScanner.first.first << " and " << overlappingScanner.first.second
                  << " are overlapping (" << overlappingScanner.second << " common beacons)\n";
    }
    std::cout << "Total number of beacons: " << numBeacons << "\n";
}
