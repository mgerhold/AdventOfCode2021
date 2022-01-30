#include "AOCUtilities.hpp"
#include <algorithm>
#include <format>
#include <iostream>
#include <unordered_set>

enum class Color {
    Light,
    Dark,
};

struct Image {
    std::unordered_set<PointI32> lightPixels;
    PointI32 min{ 0, 0 };
    PointI32 max{ 0, 0 };
    Color defaultColor;

    Image(Color defaultColor) : defaultColor{ defaultColor } {}

    [[nodiscard]] Color getPixel(i32 x, i32 y) const {
        if (x >= min.x and x <= max.x and y >= min.y and y <= max.y) {
            return lightPixels.contains(PointI32{ x, y }) ? Color::Light : Color::Dark;
        }
        return defaultColor;
    }

    void setPixel(i32 x, i32 y, Color color) {
        switch (color) {
            case Color::Light:
                if (x < min.x) {
                    min.x = x;
                }
                if (x > max.x) {
                    max.x = x;
                }
                if (y < min.y) {
                    min.y = y;
                }
                if (y > max.y) {
                    max.y = y;
                }
                lightPixels.insert(PointI32{ x, y });
                break;
            case Color::Dark:
                // can the image get smaller? who knows? do you? do i?
                std::erase_if(lightPixels, [x, y](const auto& point) {
                    return point.x == x and point.y == y;
                });
                break;
        }
    }

    [[nodiscard]] uz numLightPixels() const {
        return lightPixels.size();
    }
};

std::ostream& operator<<(std::ostream& os, const Color& color) {
    os << (color == Color::Light ? '#' : '.');
    return os;
}

std::ostream& operator<<(std::ostream& os, const Image& image) {
    for (auto y = image.min.y; y <= image.max.y; ++y) {
        for (auto x = image.min.x; x <= image.max.x; ++x) {
            os << image.getPixel(x, y);
        }
        os << "\n";
    }
    return os;
}

[[nodiscard]] Image processImage(const Image& image, const std::string_view algorithm, const i32 iteration) {
    auto newImage = Image{ iteration % 2 != 0 ? (algorithm.front() == '#' ? Color::Light : Color::Dark) : Color::Dark };
    for (auto x = image.min.x - 10; x <= image.max.x + 10; ++x) {
        for (auto y = image.min.y - 10; y <= image.max.y + 10; ++y) {
            auto base = i32{ 0b1'0000'0000 };
            auto index = i32{};
            for (auto j = -1; j <= 1; ++j) {
                for (auto i = -1; i <= 1; ++i) {
                    index += static_cast<i32>(image.getPixel(x + i, y + j) == Color::Light) * base;
                    base /= 2;
                }
            }
            // std::cout << std::format("({}, {}) => index = 0b{:09b}\n", x, y, index);
            const auto color = (algorithm.at(index) == '#' ? Color::Light : Color::Dark);
            if (color == Color::Light) {
                newImage.setPixel(x, y, color);
            }
        }
    }
    return newImage;
}

int main() {
    const auto lines = readInput("input.txt");
    const auto& algorithm = lines.front();
    auto image = Image{ Color::Dark };
    for (auto i = uz{ 2 }; i < lines.size(); ++i) {
        const auto y = static_cast<i32>(i - 2);
        for (auto x = 0; x < lines.at(i).length(); ++x) {
            if (lines.at(i).at(x) == '#') {
                image.setPixel(x, y, Color::Light);
            }
        }
    }
    constexpr auto numIterations = 50;
    //std::cout << image << "===============\n";
    for (auto iteration = 1; iteration <= numIterations; ++iteration) {
        image = processImage(image, algorithm, iteration);
        //std::cout << image << "===============\n";
    }
    std::cout << image << "\n";
    std::cout << "Number of light pixels: " << image.numLightPixels() << "\n";
}
