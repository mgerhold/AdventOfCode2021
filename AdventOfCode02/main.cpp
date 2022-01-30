#include <array>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

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

struct Vec2i {
    int x{ 0 }, y{ 0 };

    [[nodiscard]] friend Vec2i operator+(Vec2i lhs, const Vec2i& rhs) {
        return lhs += rhs;
    }

    [[nodiscard]] friend Vec2i operator*(const Vec2i& vec, const int scalar) {
        return Vec2i{ .x{ vec.x * scalar }, .y{ vec.y * scalar }};
    }

    [[nodiscard]] friend Vec2i operator*(const int scalar, const Vec2i& vec) {
        return vec * scalar;
    }

    Vec2i operator+=(const Vec2i& other) {
        Vec2i oldValue = *this;
        x += other.x;
        y += other.y;
        return oldValue;
    }
};

void part1() {
    using namespace std::string_view_literals;
    const auto input = readInput("input.txt");
    Vec2i position;
    constexpr std::array commandStrings {
            "forward"sv,
            "down"sv,
            "up"sv
    };
    constexpr std::array commandDirections {
            Vec2i{ .x{ 1 }, .y{ 0 } },
            Vec2i{ .x{ 0 }, .y{ 1 } },
            Vec2i{ .x{ 0 }, .y{ -1 } }
    };
    static_assert(commandStrings.size() == commandDirections.size());
    for (const auto& line : input) {
        for (std::size_t i = 0; i < commandStrings.size(); ++i) {
            if (line.starts_with(commandStrings[i])) {
                const auto value = std::stoi(line.substr(commandStrings[i].length() + 1));
                position += commandDirections[i] * value;
                break;
            }
        }
    }
    std::cout << "(" << position.x << ", " << position.y << ")\n";
    std::cout << "Result: " << (position.x * position.y) << "\n";
}

enum class Command {
    Forward,
    Up,
    Down,
};

[[nodiscard]] auto parseLine(const std::string& line) {
    const auto spacePosition = line.find(' ');
    Command command = Command::Forward;
    const auto commandString = line.substr(0, spacePosition);
    if (commandString == "up") {
        command = Command::Up;
    } else if (commandString == "down") {
        command = Command::Down;
    }
    return std::pair{
        command,
        std::stoi(line.substr(spacePosition + 1))
    };
}

void part2() {
    using namespace std::string_view_literals;
    const auto input = readInput("input.txt");
    // test case: expected output is (15, 60) or 900 multiplied
    /*const auto input = std::vector<std::string>{
        "forward 5",
        "down 5",
        "forward 8",
        "up 3",
        "down 8",
        "forward 2",
    };*/
    Vec2i position;
    int aim = 0;
    for (const auto& line : input) {
        const auto&&[ command, value ] = parseLine(line);
        switch (command) {
            case Command::Forward:
                position += Vec2i{ .x{ value }, .y{ aim * value }};
                break;
            case Command::Up:
                aim -= value;
                break;
            case Command::Down:
                aim += value;
                break;
        }
    }
    std::cout << "(" << position.x << ", " << position.y << ")\n";
    std::cout << "Result: " << (position.x * position.y) << "\n";
}

int main() {
    // part1();
    part2();
}
