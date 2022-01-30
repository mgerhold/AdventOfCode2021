//
// Created by coder2k on 06.12.2021.
//

#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

[[nodiscard]] inline auto readInput(const std::string& filename) {
    const auto path = std::filesystem::path{ filename };
    if (!exists(path)) {
        throw std::exception{ "The specified file does not exist. "};
    }
    std::vector<std::string> result;
    std::ifstream inputStream{ filename };
    if (!inputStream.good()) {
        throw std::exception{ "Unable to read file (maybe empty?). "};
    }
    while (inputStream.good()) {
        std::string line;
        std::getline(inputStream, line);
        result.emplace_back(std::move(line));
    }
    return result;
}

[[nodiscard]] inline std::vector<std::string> split(const std::string& s, const char delimiter = ' ') {
    std::string current;
    std::vector<std::string> result;
    for(char c : s) {
        if (c == delimiter && !current.empty()) {
            result.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        result.push_back(current);
    }
    return result;
}

[[nodiscard]] inline std::string trim(const std::string& string, const char charToTrim = ' ') {
    /* the bounds calculated describe the characters that should NOT be deleted, that's
     * why they are exclusive in the context of the characters that SHOULD be deleted */
    std::size_t leftBoundExclusive = 0;
    std::size_t rightBoundExclusive = string.length() + 1;
    for (std::size_t i = 0; i < string.length(); ++i) {
        if (string[i] != charToTrim) {
            leftBoundExclusive = i;
            break;
        }
    }
    auto index = string.length() - 1;
    for (std::size_t i = 0; i < string.length(); ++i, --index) {
        if (string[index] != charToTrim) {
            rightBoundExclusive = index;
            break;
        }
    }
    // "   hallo  "
    //  0123456789
    // leftBoundExclusive = 3
    // rightBoundExclusive = 7
    const auto length = rightBoundExclusive - leftBoundExclusive + 1;
    return string.substr(leftBoundExclusive, length);
}