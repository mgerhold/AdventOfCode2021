#include "AOCUtilities.hpp"
#include <array>
#include <iostream>
#include <optional>
#include <stack>

struct TokenPair {
    char open;
    char close;
    u64 corruptScore;
    u64 completionScore;
};

constexpr auto tokenPairs = std::array{
        TokenPair{ '(', ')', 3, 1 },
        TokenPair{ '[', ']', 57, 2 },
        TokenPair{ '{', '}', 1197, 3 },
        TokenPair{ '<', '>', 25137, 4 },
};

[[nodiscard]] std::optional<u64> getCorruptScore(const std::string& line, std::stack<char>& tokenStack) {
    auto score = u64{ 0 };
    for (const auto c : line) {
        for (auto tokenPair : tokenPairs) {
            if (c == tokenPair.open) {
                tokenStack.push(tokenPair.close);
                break;
            }
            if (c == tokenPair.close) {
                if (tokenStack.empty() || tokenStack.top() != tokenPair.close) {
                    // corrupt line
                    score += tokenPair.corruptScore;
                    return score;
                } else {
                    tokenStack.pop();
                }
            }
        }
    }
    return {};
}

[[nodiscard]] std::optional<u64> getCompletionScore(std::stack<char>& tokenStack) {
    if (tokenStack.empty()) {
        return {};
    }
    auto totalScore = u64{ 0 };
    while (!tokenStack.empty()) {
        totalScore *= 5;
        for (const auto& tokenPair : tokenPairs) {
            if (tokenStack.top() == tokenPair.close) {
                totalScore += tokenPair.completionScore;
                break;
            }
        }
        tokenStack.pop();
    }
    return totalScore;
}

int main() {
    const auto lines = readInput("input.txt");
    auto corruptScore = u64{ 0 };
    std::vector<u64> completionScores;
    for (const auto& line : lines) {
        std::stack<char> tokenStack;
        const auto currentCorruptScore = getCorruptScore(line, tokenStack);
        if (currentCorruptScore) {
            corruptScore += currentCorruptScore.value();
            continue;
        }
        const auto currentCompletionScore = getCompletionScore(tokenStack);
        if (currentCompletionScore) {
            completionScores.push_back(currentCompletionScore.value());
        }
    }
    std::cout << "Score Part 1: " << corruptScore << "\n";
    std::cout << "Score Part 2: " << median(completionScores) << "\n";
}
