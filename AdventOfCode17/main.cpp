#include "AOCUtilities.hpp"
#include <iostream>
#include <optional>
#include <ranges>
#include <cmath>

int main() {
    // testcase
    /*constexpr auto targetMinX = 20;
    constexpr auto targetMaxX = 30;
    constexpr auto targetMinY = -10;
    constexpr auto targetMaxY = -5;*/

    // actual input
    constexpr auto targetMinX = 155;
    constexpr auto targetMaxX = 215;
    constexpr auto targetMinY = -132;
    constexpr auto targetMaxY = -72;

    const auto minVelocityX = static_cast<int>(std::round(0.5 * std::sqrt(8.0 * targetMinX + 1.0) - 1.0));
    const auto maxVelocityX = targetMaxX;
    const auto maxVelocityY = std::abs(targetMinY);
    const auto minVelocityY = targetMinY;
    std::cout << "min velocity to reach target at x = " << targetMinX << " is " << minVelocityX << "\n";
    std::cout << "possible x velocity range: [" << minVelocityX << ";" << maxVelocityX << "]\n";
    std::cout << "possible y velocity range: [" << minVelocityY << ";" << maxVelocityY << "]\n";
    auto highest = std::optional<int>{};
    auto count = 0;
    using std::ranges::views::iota;
    for (const auto startingVelocityX : iota(minVelocityX, maxVelocityX + 1)) {
        for (const auto startingVelocityY : iota(minVelocityY, maxVelocityY + 1)) {
            // simulate
            auto position = Point<int>{ 0, 0 };
            auto step = Point<int>{ startingVelocityX, startingVelocityY };
            auto currentHighest = 0;
            while (position.y >= targetMinY) {
                position += step;
                if (position.y > currentHighest) {
                    currentHighest = position.y;
                }
                if (position.x >= targetMinX && position.x <= targetMaxX && position.y >= targetMinY &&
                    position.y <= targetMaxY) {
                    if (!highest || currentHighest > highest.value()) {
                        highest = currentHighest;
                    }
                    ++count;
                    break;
                }
                step = { std::max(step.x - 1, 0), step.y - 1 };
            }
        }
    }
    if (highest) {
        std::cout << "highest was " << highest.value() << "\n";
        std::cout << "number of possible starting velocities: " << count << "\n";
    }
}
