#include <algorithm>
#include <array>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <numeric>
#include <string>
#include <vector>
#include <cassert>

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

[[nodiscard]] std::vector<std::string> split(const std::string& s, const char delimiter = ' ') {
    std::string current;
    std::vector<std::string> result;
    for(char c : s) {
        if (c == delimiter && current != "") {
            result.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    if (current != "") {
        result.push_back(current);
    }
    return result;
}

struct Cell {
    std::uint8_t value{ 0 };
    bool marked{ false };
};

struct Board {
    Cell& at(const std::size_t x, const std::size_t y) {
        return cells.at(x + y * width);
    }

    [[nodiscard]] const Cell& at(const std::size_t x, const std::size_t y) const {
        return cells.at(x + y * width);
    }

    void markValue(const std::uint8_t value) {
        for (std::size_t y = 0; y < width; ++y) {
            for (std::size_t x = 0; x < width; ++x) {
                if (at(x, y).value == value) {
                    at(x, y).marked = true;
                    return;
                }
            }
        }
    }

    [[nodiscard]] bool hasWon() const {
        for (std::size_t y = 0; y < width; ++y) {
            bool success = true;
            for (std::size_t x = 0; x < width; ++x) {
                if (!at(x, y).marked) {
                    success = false;
                    break;
                }
            }
            if (success) {
                return true;
            }
        }
        for (std::size_t x = 0; x < width; ++x) {
            bool success = true;
            for (std::size_t y = 0; y < width; ++y) {
                if (!at(x, y).marked) {
                    success = false;
                    break;
                }
            }
            if (success) {
                return true;
            }
        }
        return false;
    }

    static constexpr auto width = std::size_t{ 5 };
    std::array<Cell, width * width> cells;
};

std::ostream& operator<<(std::ostream& ostream, const Board& board) {
    for (std::size_t y = 0; y < 5; ++y) {
        for (std::size_t x = 0; x < 5; ++x) {
            ostream << std::setw(4) << static_cast<int>(board.at(x, y).value);
        }
        std::cout << "\n";
    }
    return ostream;
}

std::uint32_t calculateScore(const Board& board, const std::uint8_t mostRecentNumber) {
    const auto sum = std::accumulate(board.cells.begin(), board.cells.end(), std::uint32_t{ 0 },
                                    [](const auto previous, const auto& cell) {
                                        return previous + static_cast<std::uint32_t>(cell.value) * static_cast<std::uint32_t>(!cell.marked);
                                    });
    return sum * static_cast<std::uint32_t>(mostRecentNumber);
}

void simulateGamePart01(const std::vector<std::uint8_t>& randomNumbers, std::vector<Board> boards) {
    for (const auto randomNumber : randomNumbers) {
        for (auto& board : boards) {
            board.markValue(randomNumber);
            if (board.hasWon()) {
                std::cout << "won after picking " << (int)randomNumber << "\n";
                const auto score = calculateScore(board, randomNumber);
                std::cout << "Won with score: " << score << "\n";
                return;
            }
        }
    }
}

void simulateGamePart2(const std::vector<std::uint8_t>& randomNumbers, std::vector<Board> boards) {
    for (const auto randomNumber : randomNumbers) {
        for (auto& board : boards) {
            board.markValue(randomNumber);
        }
        if (boards.size() == 1 && boards.front().hasWon()) {
            const auto score = calculateScore(boards.front(), randomNumber);
            std::cout << "Last to win with score: " << score << "\n";
            return;
        }
        std::erase_if(boards, [](const auto& board) {
            return board.hasWon();
        });
    }
}

int main() {
    const auto input = readInput("input.txt");
    const auto numberStrings = split(input.front(), ',');
    std::vector<std::uint8_t> randomNumbers;
    randomNumbers.reserve(numberStrings.size());
    for (const auto& numberString : numberStrings) {
        randomNumbers.emplace_back(static_cast<std::uint8_t>(std::stoi(numberString)));
    }
    std::size_t nextRow = 0;
    std::vector<Board> boards;
    for (std::size_t i = 1; i < input.size(); ++i) {
        if (input.at(i).empty()) {
            continue;
        }
        if (nextRow == 0) {
            boards.emplace_back(Board{});
        }
        const auto numberStringsInCurrentRow = split(input.at(i));
        assert(numberStringsInCurrentRow.size() == 5);
        for (std::size_t column = 0; column < numberStringsInCurrentRow.size(); ++column) {
            boards.back().at(column, nextRow).value = static_cast<std::uint8_t>(std::stoi(numberStringsInCurrentRow[column]));
        }
        nextRow = (nextRow + 1) % 5;
    }

    std::cout << "Boards:\n";
    for (const auto& board : boards) {
        std::cout << board << "\n";
    }

    //simulateGame(randomNumbers, boards);
    simulateGamePart2(randomNumbers, boards);
}
