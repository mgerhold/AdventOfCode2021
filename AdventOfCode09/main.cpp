#include "AOCUtilities.hpp"
#include <algorithm>
#include <array>
#include <format>
#include <iostream>
#include <unordered_set>
#include <vector>


struct Point {
    uz x;
    uz y;

    [[nodiscard]] bool operator==(const Point &) const = default;
};

namespace std {
    template<>
    struct hash<Point> {
        uz operator()(const Point &point) const {
            std::hash<uz> hasher;
            const auto seed = hasher(point.x);
            return hasher(point.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };
}

class Map {
public:
    Map(uz width, uz height) {
        mTiles.resize(width * height);
        mWidth = width;
    }

    [[nodiscard]] static Map fromVector(const std::vector<std::string> &vector) {
        const auto width = vector.front().length();
        const auto height = vector.size();
        auto result = Map{width, height};
        for (uz y = 0; y < height; ++y) {
            const auto &line = vector.at(y);
            for (uz x = 0; x < width; ++x) {
                result.at(x, y) = static_cast<u8>(line.at(x) - '0');
            }
        }
        return result;
    }

    // part 1
    [[nodiscard]] u32 calculateRiskLevelsOfLowPoints() const {
        auto riskLevels = u32{0};
        for (uz y = 0; y < height(); ++y) {
            for (uz x = 0; x < mWidth; ++x) {
                constexpr auto highestPlusOne = u8{10};
                const auto neighbors = std::array{
                        areCoordinatesValid(x, y - 1) ? at(x, y - 1) : highestPlusOne, // up
                        areCoordinatesValid(x, y + 1) ? at(x, y + 1) : highestPlusOne, // down
                        areCoordinatesValid(x - 1, y) ? at(x - 1, y) : highestPlusOne, // left
                        areCoordinatesValid(x + 1, y) ? at(x + 1, y) : highestPlusOne, // right
                };
                const auto tileHeight = at(x, y);
                const auto lowestTileAmongNeighbors = std::all_of(begin(neighbors), end(neighbors),
                                                                  [tileHeight](const auto neighborHeight) {
                                                                      return neighborHeight > tileHeight;
                                                                  });
                riskLevels += static_cast<u32>(tileHeight + 1) * static_cast<u32>(lowestTileAmongNeighbors);
            }
        }
        return riskLevels;
    }

    // part 2
    [[nodiscard]] u32 productOfThreeGreatestBasins() const {
        std::vector<uz> basinSizes;
        for (uz y = 0; y < height(); ++y) {
            for (uz x = 0; x < mWidth; ++x) {
                constexpr auto highestPlusOne = u8{10};
                const auto neighbors = std::array{
                        areCoordinatesValid(x, y - 1) ? at(x, y - 1) : highestPlusOne, // up
                        areCoordinatesValid(x, y + 1) ? at(x, y + 1) : highestPlusOne, // down
                        areCoordinatesValid(x - 1, y) ? at(x - 1, y) : highestPlusOne, // left
                        areCoordinatesValid(x + 1, y) ? at(x + 1, y) : highestPlusOne, // right
                };
                const auto tileHeight = at(x, y);
                const auto lowestTileAmongNeighbors = std::all_of(begin(neighbors), end(neighbors),
                                                                  [tileHeight](const auto neighborHeight) {
                                                                      return neighborHeight > tileHeight;
                                                                  });
                if (lowestTileAmongNeighbors) {
                    // find size of basin
                    std::vector<Point> toVisit{Point{x, y}};
                    std::unordered_set<Point> visited;
                    std::vector<Point> potentialNeighbors;
                    potentialNeighbors.reserve(4);
                    while (!toVisit.empty()) {
                        const auto point = toVisit.back();
                        toVisit.pop_back();
                        visited.insert(point);

                        potentialNeighbors.clear();
                        potentialNeighbors.push_back(Point{point.x, point.y - 1}); // up
                        potentialNeighbors.push_back(Point{point.x, point.y + 1}); // down
                        potentialNeighbors.push_back(Point{point.x + 1, point.y}); // right
                        potentialNeighbors.push_back(Point{point.x - 1, point.y}); // left

                        for (const auto &neighbor: potentialNeighbors) {
                            if (areCoordinatesValid(neighbor.x, neighbor.y) && at(neighbor.x, neighbor.y) < 9 &&
                                !visited.contains(neighbor)) {
                                toVisit.emplace_back(neighbor);
                            }
                        }
                    }
                    std::cout << std::format("Found basin with size {}\n", visited.size());
                    basinSizes.emplace_back(visited.size());
                }
            }
        }
        //std::partial_sort(begin(basinSizes), begin(basinSizes) + 3, end(basinSizes), std::greater{});
        std::nth_element(begin(basinSizes), begin(basinSizes) + 3, end(basinSizes), std::greater{});
        return basinSizes.front() * basinSizes.at(1) * basinSizes.at(2);
    }

    u8 &at(uz x, uz y) {
        return mTiles.at(x + y * mWidth);
    }

    [[nodiscard]] u8 at(uz x, uz y) const {
        return mTiles.at(x + y * mWidth);
    }

    [[nodiscard]] uz width() const {
        return mWidth;
    }

    [[nodiscard]] uz height() const {
        return mTiles.size() / mWidth;
    }

private:
    [[nodiscard]] bool areCoordinatesValid(uz x, uz y) const {
        const auto result = (x < mWidth && y < height());
        return result;
    }

private:
    uz mWidth;
    std::vector<u8> mTiles;
};

std::ostream &operator<<(std::ostream &ostream, const Map &map) {
    for (uz y = 0; y < map.height(); ++y) {
        for (uz x = 0; x < map.width(); ++x) {
            ostream << static_cast<u32>(map.at(x, y));
        }
        ostream << "\n";
    }
    return ostream;
}

int main() {
    const auto map = Map::fromVector(readInput("input.txt"));
    // std::cout << map << "\n";

    // part 1
    std::cout << std::format("Result: {}\n", map.calculateRiskLevelsOfLowPoints());
    // part 2
    std::cout << std::format("Product of three largest basins: {}\n", map.productOfThreeGreatestBasins());
}
