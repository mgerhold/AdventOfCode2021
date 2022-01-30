#include "AOCUtilities.hpp"
#include <array>
#include <chrono>
#include <iostream>
#include <compare>
#include <optional>
#include <string>
#include <unordered_set>
#include <queue>
#include <vector>
#include <cassert>

#define PART2

struct WeightedTile {
    uz x, y, cost, prevX, prevY;

    [[nodiscard]] auto operator<=>(const WeightedTile& other) const {
        return cost <=> other.cost;
    }

    [[nodiscard]] bool operator==(const WeightedTile& other) const {
        return x == other.x && y == other.y;
    }
};

namespace std {
    template<>
    struct hash<WeightedTile> {
        uz operator()(const WeightedTile& weightedTile) const {
            return combineHashes(weightedTile.x, weightedTile.y);
        }
    };
}// namespace std

class Map {
public:
    Map(uz width, uz height) : mWidth{ width } {
        mTiles.resize(width * height);
    }

    [[nodiscard]] static Map fromFilePart1(const std::string& filename) {
        const auto lines = readInput(filename);
        auto result = Map{ lines.front().length(), lines.size() };
        for (auto y = uz{ 0 }; y < lines.size(); ++y) {
            const auto& line = lines[y];
            for (auto x = uz{ 0 }; x < line.length(); ++x) {
                result.at(x, y) = static_cast<u8>(line[x] - '0');
            }
        }
        return result;
    }

    [[nodiscard]] static Map fromFilePart2(const std::string& filename) {
        const auto lines = readInput(filename);
        static constexpr auto scalingFactor = 5;
        const auto subMapWidth = lines.front().length();
        const auto subMapHeight = lines.size();
        auto result = Map{ subMapWidth * scalingFactor, subMapHeight * scalingFactor };
        for (auto y = uz{ 0 }; y < subMapHeight; ++y) {
            const auto& line = lines[y];
            for (auto x = uz{ 0 }; x < subMapWidth; ++x) {
                result.at(x, y) = static_cast<u8>(line[x] - '0');
            }
        }

        for (auto i = 0; i < scalingFactor; ++i) {
            for (auto j = 0; j < scalingFactor; ++j) {
                if (i == 0 && j == 0) {
                    continue;
                }
                for (auto y = uz{ 0 }; y < subMapHeight; ++y) {
                    for (auto x = uz{ 0 }; x < subMapWidth; ++x) {
                        assert(i + j < 9);// :)
                        auto newValue = (result.at(x, y) + i + j);
                        if (newValue > 9) {
                            newValue %= 9;
                        }
                        result.at(x + subMapWidth * i, y + subMapHeight * j) = static_cast<u8>(newValue);
                    }
                }
            }
        }
        return result;
    }

    void printPath(const auto& visited) {
        auto destination = PointUZ{ width() - 1, height() - 1 };
        auto result = std::optional<uz>{};
        std::vector<PointUZ> path;
        while (true) {// Andrei Alexandrescu approved
            const auto destinationIterator = std::find_if(
                    visited.begin(), visited.end(),
                    [&destination](const auto& tile) { return tile.x == destination.x && tile.y == destination.y; });
            assert(destinationIterator != visited.end());
            if (!result) {
                result = destinationIterator->cost;
            }
            path.emplace_back(destination.x, destination.y);
            destination.x = destinationIterator->prevX;
            destination.y = destinationIterator->prevY;
            if (destination.x == 0 && destination.y == 0) {
                break;
            }
        }
        path.emplace_back(0, 0);
        for (auto i = uz{ 0 }; i < path.size(); ++i) {
            std::cout << "(" << path[path.size() - i - 1].x << "," << path[path.size() - i - 1].y << ")\n";
        }
    }

    [[nodiscard]] uz calculateMinCost() {
        std::unordered_set<WeightedTile> visited;
        std::vector<WeightedTile> toVisit;
        visited.insert(WeightedTile{ 0, 0, 0, 0, 0 });
        toVisit.push_back(WeightedTile{ 1, 0, at(1, 0), 0, 0 });
        toVisit.push_back(WeightedTile{ 0, 1, at(0, 1), 0, 0 });
        std::make_heap(toVisit.begin(), toVisit.end(), std::greater{});
        const auto totalNumTiles = width() * height();
        while (!toVisit.empty()) {
            const auto currentTile = toVisit.front();
            std::pop_heap(toVisit.begin(), toVisit.end(), std::greater{});
            toVisit.pop_back();
            visited.insert(currentTile);
            const auto possibleNeighbors = std::array{
                PointUZ{ currentTile.x + 1, currentTile.y },// right
                PointUZ{ currentTile.x - 1, currentTile.y },// left
                PointUZ{ currentTile.x, currentTile.y - 1 },// up
                PointUZ{ currentTile.x, currentTile.y + 1 },// down
            };
            for (const auto& neighbor : possibleNeighbors) {
                if (!isValidCoordinate(neighbor) || visited.contains(WeightedTile{ neighbor.x, neighbor.y, 0, 0, 0 })) {
                    continue;
                }
                const auto findIterator = std::find_if(toVisit.begin(), toVisit.end(), [&neighbor](const auto& tile) {
                    return neighbor.x == tile.x && neighbor.y == tile.y;
                });
                const auto newCost = currentTile.cost + at(neighbor.x, neighbor.y);
                if (findIterator != toVisit.end()) {
                    if (newCost < findIterator->cost) {
                        // update existing tile if newly found path is cheaper
                        findIterator->cost = newCost;
                        std::make_heap(toVisit.begin(), toVisit.end(), std::greater{});
                    }
                } else {
                    // add new tile
                    toVisit.push_back(WeightedTile{ neighbor.x, neighbor.y, newCost, currentTile.x, currentTile.y });
                    std::push_heap(toVisit.begin(), toVisit.end(), std::greater{});
                }
            }
        }
        // printPath(visited);
        const auto destination = PointUZ{ width() - 1, height() - 1 };
        const auto destinationIterator = std::find_if(visited.begin(), visited.end(), [&destination](const auto& tile) {
            return tile.x == destination.x && tile.y == destination.y;
        });
        return destinationIterator->cost;
    }

    u8& at(uz x, uz y) {
        return mTiles.at(x + y * width());
    }

    [[nodiscard]] u8 at(uz x, uz y) const {
        return mTiles.at(x + y * width());
    }

    [[nodiscard]] uz width() const {
        return mWidth;
    }

    [[nodiscard]] uz height() const {
        return mTiles.size() / width();
    }

private:
    [[nodiscard]] bool isValidCoordinate(uz x, uz y) const {
        return x < width() && y < height();
    }

    [[nodiscard]] bool isValidCoordinate(PointUZ point) const {
        return isValidCoordinate(point.x, point.y);
    }

private:
    uz mWidth;
    std::vector<u8> mTiles;
};

std::ostream& operator<<(std::ostream& ostream, const Map& map) {
    for (auto y = uz{ 0 }; y < map.height(); ++y) {
        for (auto x = uz{ 0 }; x < map.width(); ++x) {
            ostream << static_cast<u16>(map.at(x, y));
        }
        ostream << "\n";
    }
    return ostream;
}

int main() {
#ifndef PART2
    auto map = Map::fromFilePart1("input.txt");
    const auto minCost = map.calculateMinCost();
    std::cout << "min cost: " << minCost << "\n";
#else
    auto map = Map::fromFilePart2("input.txt");
    const auto startTime = std::chrono::high_resolution_clock::now();
    const auto minCost = map.calculateMinCost();
    const auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "min cost: " << minCost << " (took " << std::chrono::duration<double>(endTime - startTime) << ")\n";
#endif
}
