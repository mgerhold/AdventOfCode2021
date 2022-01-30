#include "AOCUtilities.hpp"
#include <algorithm>
#include <array>
#include <iostream>
#include <unordered_map>
#include <cassert>
#include <cstdint>

using u32 = std::uint32_t;
using u64 = std::uint64_t;

#define PART2

struct Point {
    u32 x, y;

    [[nodiscard]] constexpr bool operator==(const Point&) const = default;
};

std::ostream& operator<<(std::ostream& ostream, const Point& point) {
    ostream << "(" << point.x << "," << point.y << ")";
    return ostream;
}

namespace std {
    template<>
    struct hash<Point> {
        std::size_t operator()(const Point& point) const {
            using std::hash;
            return hash<std::size_t>()(static_cast<u64>(point.x) << 32 | static_cast<u64>(point.y));
        }
    };
}

class Map {
public:
    [[nodiscard]] u32 get(const Point coordinates) const {
        return (mTiles.contains(coordinates) ? mTiles.at(coordinates) : 0);
    }

    void increase(const Point coordinates) {
        if (coordinates.x >= mWidth) {
            mWidth = coordinates.x + 1;
        }
        if (coordinates.y >= mHeight) {
            mHeight = coordinates.y + 1;
        }
        mTiles[coordinates] = get(coordinates) + 1;
    }

    [[nodiscard]] std::size_t width() const {
        return mWidth;
    }

    [[nodiscard]] std::size_t height() const {
        return mHeight;
    }

    [[nodiscard]] std::size_t countTilesGreaterOrEqual(const u32 threshold) const {
        return std::count_if(mTiles.begin(), mTiles.end(), [threshold](const auto& tile) {
            return tile.second >= threshold;
        });
    }


private:
    std::size_t mWidth{ 0 };
    std::size_t mHeight{ 0 };
    std::unordered_map<Point, u32> mTiles;
};

std::ostream& operator<<(std::ostream& ostream, const Map& map) {
    for (u32 y = 0; y < map.height(); ++y) {
        for (u32 x = 0; x < map.width(); ++x) {
            const auto tile = map.get({ x, y });
            ostream << (tile == 0 ? '.' : static_cast<char>('0' + tile % 10));
        }
        ostream << '\n';
    }
    return ostream;
}

Map inputAsMap(const std::vector<std::string>& input) {
    Map result;
    for (const auto& line : input) {
        const auto parts = split(line, ' ');
        assert(parts.size() == 3);
        std::array pointParts{
            split(parts[0], ','),
            split(parts[2], ',')
        };
        std::array<Point, 2> points{};
        static_assert(pointParts.size() == points.size());
        for (std::size_t i = 0; i < pointParts.size(); ++i) {
            assert(pointParts[i].size() == 2);
            points[i] = { std::stoul(pointParts[i][0]), std::stoul(pointParts[i][1]) };
        }
        const auto sameX = (points[0].x == points[1].x);
        const auto sameY = (points[0].y == points[1].y);
        if (sameX && !sameY) {
            const auto minY = std::min(points[0].y, points[1].y);
            const auto maxY = std::max(points[0].y, points[1].y);
            for (u32 y = minY; y <= maxY; ++y) {
                result.increase({ points[0].x, y });
            }
            continue;
        } else if (!sameX && sameY) {
            const auto minX = std::min(points[0].x, points[1].x);
            const auto maxX = std::max(points[0].x, points[1].x);
            for (u32 x = minX; x <= maxX; ++x) {
                result.increase({x, points[0].y});
            }
            continue;
        }
#ifdef PART2
        // diagonal
        const auto leftPoint = (points[0].x < points[1].x ? points[0] : points[1]);
        const auto rightPoint = (points[0].x < points[1].x ? points[1] : points[0]);
        const auto increment = (leftPoint.y < rightPoint.y ? 1 : -1);
        auto y = leftPoint.y;
        for (u32 x = leftPoint.x; x <= rightPoint.x; ++x, y = static_cast<u32>(static_cast<int>(y) + increment)) {
            result.increase({ x, y });
        }
#endif
    }
    return result;
}

int main() {
    const auto map = inputAsMap(readInput("input.txt"));
    std::cout << map;
    std::cout << "Number of tiles greater than or equal to 2: " << map.countTilesGreaterOrEqual(2) << "\n";
}