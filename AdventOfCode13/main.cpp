#include "AOCUtilities.hpp"
#include <iostream>
#include <unordered_set>
#include <deque>

enum class FoldType {
    Horizontal,
    Vertical,
};

struct Fold {
    FoldType type;
    u32 destination;
};

class Paper {
public:
    bool applyFold() {
        const auto& fold = mFolds.front();
        if (fold.type == FoldType::Horizontal) {
            for (const auto& dot : mDots) {
                if (dot.y > fold.destination) {
                    mDots.insert({ dot.x, 2 * fold.destination - dot.y });
                }
            }
            erase_if(mDots, [&fold] (const auto& dot) {
                     return dot.y > fold.destination;
                 });
            mSize.y /= 2;
        } else {
            // vertical fold
            for (const auto& dot : mDots) {
                if (dot.x > fold.destination) {
                    mDots.insert({ 2 * fold.destination - dot.x, dot.y });
                }
            }
            erase_if(mDots, [&fold] (const auto& dot) {
                return dot.x > fold.destination;
            });
            mSize.x /= 2;
        }
        mFolds.pop_front();
        return !mFolds.empty();
    }

    [[nodiscard]] uz numDots() const {
        return mDots.size();
    }

    [[nodiscard]] static Paper fromFile(const std::string& filename) {
        auto result = Paper{};
        const auto lines = readInput(filename);
        auto readingMarks = true;
        for (const auto& line : lines) {
            if (line.empty()) {
                readingMarks = false;
                continue;
            }
            if (readingMarks) {
                const auto parts = split(line, ',');
                auto point = PointType{ std::stoul(parts[0]), std::stoul(parts[1]) };
                result.mSize.x = std::max(result.mSize.x, point.x + 1);
                result.mSize.y = std::max(result.mSize.y, point.y + 1);
                result.markDot(point);
            } else {
                using namespace std::string_view_literals;
                static constexpr auto prefixLength = ("fold along "sv).length();
                const auto parts = split(line.substr(prefixLength), '=');
                result.mFolds.emplace_back(parts[0] == "x" ? FoldType::Vertical : FoldType::Horizontal,
                                           std::stoul(parts[1]));
            }
        }
        return result;
    }

    friend std::ostream& operator<<(std::ostream& ostream, const Paper& paper) {
        for (auto y = u32{ 0 }; y < paper.mSize.y; ++y) {
            for (auto x = u32{ 0 }; x < paper.mSize.x; ++x) {
                ostream << (paper.mDots.contains({ x, y }) ? "■" : " ");
            }
            ostream << '\n';
        }
        ostream << "\nFolds:\n";
        for (const auto& fold : paper.mFolds) {
            ostream << (fold.type == FoldType::Horizontal ? 'y' : 'x') << " = " << fold.destination << "\n";
        }
        return ostream;
    }

private:
    using PointType = PointU32;

    void markDot(const PointType& point) {
        mDots.insert(point);
    }

private:
    std::unordered_set<PointType> mDots;
    PointType mSize{ 0, 0 };
    std::deque<Fold> mFolds;
};

int main() {
    auto paper = Paper::fromFile("input.txt");
    std::cout << paper << "\n";
    while (paper.applyFold()) { }
    std::cout << paper << "\n";
    std::cout << "Num dots: " << paper.numDots() << "\n";
}
