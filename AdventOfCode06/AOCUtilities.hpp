//
// Created by coder2k on 06.12.2021.
//

#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

[[nodiscard]] auto inline readInput(const std::string& filename) {
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

[[nodiscard]] std::vector<std::string> inline split(const std::string& s, const char delimiter = ' ') {
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