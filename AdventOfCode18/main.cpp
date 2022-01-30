#include "AOCUtilities.hpp"
#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <stack>
#include <string_view>
#include <vector>

struct Token {
    [[nodiscard]] virtual std::string stringRepresentation() const = 0;
};

struct Bracket : public Token {
    explicit Bracket(int level) : level{ level } { }

    int level;
};

struct OpeningBracket : public Bracket {
    using Bracket::Bracket;

    [[nodiscard]] std::string stringRepresentation() const override {
        return "[";
        // return "[\n" + std::string(static_cast<uz>(level + 1), '\t');
    }
};

struct ClosingBracket : public Bracket {
    using Bracket::Bracket;

    [[nodiscard]] std::string stringRepresentation() const override {
        return "]";
        //return "\n" + std::string(static_cast<uz>(level), '\t') + "]";
    }
};

struct RegularNumber : public Token {
    explicit RegularNumber(int value) : value{ value } { }

    [[nodiscard]] std::string stringRepresentation() const override {
        return std::to_string(value);
    }

    int value;
};

struct Comma : public Token {
    [[nodiscard]] std::string stringRepresentation() const override {
        return ",";
    }
};

using TokenList = std::vector<std::shared_ptr<Token>>;

struct SnailfishNumber {
    // Warning: This struct DOES NOT implement a proper copy constructor!
    // copying instances of this type and changing the contents does also change the
    // copied-from object (due to the shared_ptr)

    [[nodiscard]] SnailfishNumber operator+(const SnailfishNumber& other) const {
        auto lhs = *this;
        auto rhs = other;
        for (auto& token : lhs.tokens) {
            auto openBracketPtr = std::dynamic_pointer_cast<OpeningBracket>(token);
            if (openBracketPtr) {
                ++(openBracketPtr->level);
                continue;
            }
            auto closingBracketPtr = std::dynamic_pointer_cast<ClosingBracket>(token);
            if (closingBracketPtr) {
                ++(closingBracketPtr->level);
            }
        }
        for (auto& token : rhs.tokens) {
            auto openBracketPtr = std::dynamic_pointer_cast<OpeningBracket>(token);
            if (openBracketPtr) {
                ++(openBracketPtr->level);
                continue;
            }
            auto closingBracketPtr = std::dynamic_pointer_cast<ClosingBracket>(token);
            if (closingBracketPtr) {
                ++(closingBracketPtr->level);
            }
        }
        auto result = SnailfishNumber{};
        result.tokens.push_back(std::make_shared<OpeningBracket>(0));
        for (const auto& token : lhs.tokens) {
            result.tokens.push_back(token);
        }
        result.tokens.push_back(std::make_shared<Comma>());
        for (const auto& token : rhs.tokens) {
            result.tokens.push_back(token);
        }
        result.tokens.push_back(std::make_shared<ClosingBracket>(0));
        result.reduce();
        return result;
    }

    void explode(const auto openingBracketIterator) {
        /* Iterator points to the opening bracket of the pair to explode
         * ... [ L , R ] ...
         * ... 0 1 2 3 4 ... <- relative offsets to iterator
         */
        const auto pair =
                std::array{ std::static_pointer_cast<RegularNumber>(*std::next(openingBracketIterator, 1))->value,
                            std::static_pointer_cast<RegularNumber>(*std::next(openingBracketIterator, 3))->value };
        // find first regular number to the left
        for (auto iterator = std::next(openingBracketIterator, -1); iterator > std::begin(tokens); --iterator) {
            const auto regularNumberPtr = std::dynamic_pointer_cast<RegularNumber>(*iterator);
            if (regularNumberPtr) {
                // add left pair value to this regular number
                regularNumberPtr->value += pair.at(0);
                break;
            }
        }

        // find first regular number to the right of the pair
        for (auto iterator = std::next(openingBracketIterator, 5); iterator < std::end(tokens); ++iterator) {
            const auto regularNumberPtr = std::dynamic_pointer_cast<RegularNumber>(*iterator);
            if (regularNumberPtr) {
                // add right pair value to this regular number
                regularNumberPtr->value += pair.at(1);
                break;
            }
        }

        // replace exploded pair with 0
        // first erase everything of the pair except the opening bracket
        tokens.erase(std::next(openingBracketIterator, 1), std::next(openingBracketIterator, 5));
        // then replace the opening bracket with the regular number 0
        *openingBracketIterator = std::make_shared<RegularNumber>(0);
    }

    void split(const auto numberIterator) {
        // calculate the contents of the pair to be created
        const auto pair = std::array{ std::static_pointer_cast<RegularNumber>(*numberIterator)->value / 2,
                                      (std::static_pointer_cast<RegularNumber>(*numberIterator)->value + 1) / 2 };

        // find closest bracket to determine nesting-level
        const int surroundingLevel = [&]() {
            const auto leftNeighborPtr = std::dynamic_pointer_cast<OpeningBracket>(*std::next(numberIterator, -1));
            if (leftNeighborPtr) {
                return leftNeighborPtr->level;
            }
            return std::static_pointer_cast<OpeningBracket>(*std::next(numberIterator))->level;
        }();

        // replace the number with an opening bracket
        *numberIterator = std::make_shared<OpeningBracket>(surroundingLevel + 1);
        // get position inside the container (because iterators are going to be invalidated while inserting)
        // and then insert the rest of the pair
        const auto numberPosition = std::distance(std::begin(tokens), numberIterator);
        tokens.insert(std::next(std::begin(tokens), numberPosition + 1), std::make_shared<RegularNumber>(pair.at(0)));
        tokens.insert(std::next(std::begin(tokens), numberPosition + 2), std::make_shared<Comma>());
        tokens.insert(std::next(std::begin(tokens), numberPosition + 3), std::make_shared<RegularNumber>(pair.at(1)));
        tokens.insert(std::next(std::begin(tokens), numberPosition + 4),
                      std::make_shared<ClosingBracket>(surroundingLevel + 1));
    }

    void reduce() {
        bool canBeReducedFurther = true;
        while (canBeReducedFurther) {
            // check for level-4-nesting
            const auto deeplyNestedIterator = std::find_if(std::begin(tokens), std::end(tokens), [](const auto& token) {
                const auto openingBracketPtr = std::dynamic_pointer_cast<OpeningBracket>(token);
                return openingBracketPtr && openingBracketPtr->level >= 4;
            });
            if (deeplyNestedIterator != std::end(tokens)) {
                explode(deeplyNestedIterator);
                continue;
            }
            const auto highRegularNumberIterator =
                    std::find_if(std::begin(tokens), std::end(tokens), [](const auto& token) {
                        const auto regularNumberPtr = std::dynamic_pointer_cast<RegularNumber>(token);
                        return regularNumberPtr && regularNumberPtr->value >= 10;
                    });
            if (highRegularNumberIterator != std::end(tokens)) {
                split(highRegularNumberIterator);
                continue;
            }
            canBeReducedFurther = false;
        }
    }

    [[nodiscard]] int magnitude() const {
        auto stack = std::stack<int>{};
        for (const auto& token : tokens) {
            const auto numberPtr = std::dynamic_pointer_cast<RegularNumber>(token);
            if (numberPtr) {
                stack.push(numberPtr->value);
                continue;
            }
            const auto closingBracketPtr = std::dynamic_pointer_cast<ClosingBracket>(token);
            if (closingBracketPtr) {
                const auto rhs = stack.top();
                stack.pop();
                const auto lhs = stack.top();
                stack.pop();
                stack.push(3 * lhs + 2 * rhs);
            }
        }
        assert(stack.size() == 1);
        return stack.top();
    }

    TokenList tokens;
};

[[nodiscard]] SnailfishNumber parseSnailfishNumber(const std::string_view input) {
    int level = -1;
    auto result = SnailfishNumber{};
    auto currentNumString = std::string{};
    for (const auto c : input) {
        if (std::isdigit(c)) {
            currentNumString += c;
            continue;
        } else if (!currentNumString.empty()) {
            result.tokens.push_back(std::make_shared<RegularNumber>(std::stoi(currentNumString)));
            currentNumString.clear();
        }

        if (c == '[') {
            ++level;
            result.tokens.push_back(std::make_shared<OpeningBracket>(level));
        } else if (c == ']') {
            result.tokens.push_back(std::make_shared<ClosingBracket>(level));
            --level;
        } else if (c == ',') {
            result.tokens.push_back(std::make_shared<Comma>());
        }
    }
    return result;
}

std::ostream& operator<<(std::ostream& ostream, const SnailfishNumber& number) {
    for (const auto& token : number.tokens) {
        ostream << token->stringRepresentation();
    }
    return ostream;
}

int main() {
    const auto input = readInput("input.txt");
    const auto numbers = [&]() {
        auto result = std::vector<SnailfishNumber>{};
        result.reserve(input.size());
        for (const auto& line : input) {
            result.push_back(parseSnailfishNumber(line));
        }
        return result;
    }();

    auto result = numbers.front();
    for (auto iterator = std::next(numbers.begin()); iterator < std::end(numbers); ++iterator) {
        result = result + *iterator;
    }
    std::cout << "Sum of all numbers: " << result << "\n";
    std::cout << "Magnitude: " << result.magnitude() << "\n";

    auto max = std::optional<int>{};
    for (auto i = uz{ 0 }; i < numbers.size(); ++i) {
        for (auto j = uz{ 0 }; j < numbers.size(); ++j) {
            if (i == j) {
                continue;
            }
            // numbers have to be parsed again because of the faulty copy-semantics of the SnailfishNumber struct
            const auto sum = parseSnailfishNumber(input.at(i)) + parseSnailfishNumber(input.at(j));
            const auto magnitude = sum.magnitude();
            if (!max || magnitude > max.value()) {
                max = magnitude;
            }
        }
    }
    assert(max);
    std::cout << "Maximum possible sum: " << max.value() << "\n";
}
