#include "AOCUtilities.hpp"
#include "expected.hpp"
#include <iostream>
#include <array>
#include <numeric>
#include <ranges>
#include <string>
#include <string_view>
#include <variant>
#include <cctype>
#include <cassert>

std::string inline decodeHexadecimal(char hex) {
    static constexpr auto decoded = std::array{
        "0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
        "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111",
    };
    hex = static_cast<char>(std::toupper(hex));
    assert(std::isxdigit(hex));
    return decoded[hex <= '9' ? hex - '0' : hex + 10 - 'A'];
}

struct LiteralPacket {
    u64 version;
    u64 type{ 4 };
    u64 value;
};

struct ResultDescription;

struct OperatorPacket {
    u64 version;
    u64 type;
    std::vector<ResultDescription> subPacketsInfo;
};

struct ResultDescription {
    std::variant<std::string_view, u64, LiteralPacket, OperatorPacket, std::vector<ResultDescription>> consumed;
    std::string_view remainder;
};

using Input = std::string_view;
using ParseResult = tl::expected<ResultDescription, std::string_view>;
using ParseError = tl::unexpected<std::string_view>;

u64 inline binaryToDecimal(const std::string_view binaryString) {
    auto result = u64{};
    auto factor = u64{ 1 };
    for (char digit : std::ranges::reverse_view(binaryString)) {
        assert(digit == '0' || digit == '1');
        result += (digit - '0') * factor;
        factor *= 2;
    }
    return result;
}

auto inline parseAnyBit() {
    return [](const Input input) -> ParseResult {
        if (input.empty() || (input.front() != '0' && input.front() != '1')) {
            return ParseError{ "unable to parse bit" };
        }
        return ResultDescription{ input.substr(0, 1), input.substr(1) };
    };
}

auto inline parseBit(bool bitValue) {
    return [bitValue](const Input input) -> ParseResult {
        const auto result = parseAnyBit()(input);
        if (!result) {
            return result;
        }
        if (static_cast<bool>(get<std::string_view>(result->consumed).front() - '0') == bitValue) {
            return result;
        } else {
            return ParseError{ "unexpected bit value found" };
        }
    };
}

auto inline parseAnyBits(uz numBits) {
    return [numBits](const Input input) -> ParseResult {
        auto remainder = input;
        for (auto i = uz{ 0 }; i < numBits; ++i) {
            ParseResult result;
            if (!(result = parseAnyBit()(remainder))) {
                return result;
            }
            remainder = remainder.substr(1);
        }
        return ResultDescription{ input.substr(0, numBits), input.substr(numBits) };
    };
}

auto inline parseAnyNumber(uz numBits) {
    return [numBits](const Input input) -> ParseResult {
        const auto result = parseAnyBits(numBits)(input);
        if (!result) {
            return result;
        }
        return ResultDescription{ binaryToDecimal(get<std::string_view>(result->consumed)), result->remainder };
    };
}

auto inline parseNumber(u64 number, uz numBits) {
    return [number, numBits](const Input input) -> ParseResult {
        const auto result = parseAnyNumber(numBits)(input);
        if (!result) {
            return result;
        }
        if (get<u64>(result->consumed) != number) {
            return ParseError{ "found unexpected number" };
        }
        return result;
    };
}

auto parseAllOf(auto... parsers) {
    return [parsers...](const Input input) -> ParseResult {
        auto remainder = input;
        auto results = std::vector<ResultDescription>{};
        auto success = ([&]() -> bool {
            const auto result = parsers(remainder);
            if (!result) {
                return false;
            }
            remainder = result->remainder;
            results.push_back(ResultDescription{ result->consumed, result->remainder });
            return true;
        }() && ...);
        if (!success) {
            return ParseError{ "parseAllOf failed" };
        }
        return ResultDescription{ std::move(results), remainder };
    };
}

auto parseAnyOf(auto... parsers) {
    return [parsers...](const Input input) -> ParseResult {
        auto result = ParseResult{};
        auto success = ([&]() -> bool {
            const auto parseResult = parsers(input);
            if (!parseResult) {
                return false;
            }
            result = parseResult;
            return true;
        }() || ...);
        if (success) {
            return result;
        }
        return ParseError{ "parseAnyOf failed" };
    };
}

auto parseNTimes(auto parser, uz count) {
    return [parser, count](const Input input) -> ParseResult {
        auto results = std::vector<ResultDescription>{};
        auto remainder = input;
        for (auto i = uz{ 0 }; i < count; ++i) {
            auto parseResult = parser(remainder);
            if (!parseResult) {
                return parseResult;
            }
            remainder = parseResult->remainder;
            results.push_back(*parseResult);
        }
        return ResultDescription{ std::move(results), remainder };
    };
}

auto parseOnceOrMore(auto parser) {
    return [parser](const Input input) -> ParseResult {
        auto result = parser(input);
        if (!result) {
            return result;
        }
        auto results = std::vector<ResultDescription>{};
        auto remainder = result->remainder;
        results.push_back(std::move(*result));
        while (true) {
            result = parser(remainder);
            if (!result) {
                break;
            }
            remainder = result->remainder;
            results.push_back(std::move(*result));
        }
        return ResultDescription{ std::move(results), remainder };
    };
}

auto inline parseAnyLiteralPacket() {
    return [](const Input input) -> ParseResult {
        auto result = LiteralPacket{};
        auto versionParseResult = parseAnyNumber(3)(input);
        if (!versionParseResult) {
            return versionParseResult;
        }
        result.version = get<u64>(versionParseResult->consumed);
        auto typeParseResult = parseNumber(4, 3)(versionParseResult->remainder);
        if (!typeParseResult) {
            return typeParseResult;
        }
        result.type = 4;
        auto binaryString = std::string{};
        auto continueParsing = true;
        auto remainder = typeParseResult->remainder;
        do {
            auto bitsResult = parseAnyBits(5)(remainder);
            if (!bitsResult) {
                return bitsResult;
            }
            const auto consumed = get<std::string_view>(bitsResult->consumed);
            continueParsing = consumed.front() == '1';
            binaryString += consumed.substr(1);
            remainder = bitsResult->remainder;
        } while (continueParsing);
        result.value = binaryToDecimal(binaryString);
        return ResultDescription{ result, remainder };
    };
}

struct ParseAnyOperatorPacket {
    ParseResult operator()(Input input) const;
};

auto inline parseAnyPacket() {
    return parseAnyOf(parseAnyLiteralPacket(), ParseAnyOperatorPacket{});
}

ParseResult inline ParseAnyOperatorPacket::operator()(const Input input) const {
    auto result = OperatorPacket{};
    auto versionParseResult = parseAnyNumber(3)(input);
    if (!versionParseResult) {
        return versionParseResult;
    }
    result.version = get<u64>(versionParseResult->consumed);
    auto typeParseResult = parseAnyNumber(3)(versionParseResult->remainder);
    if (!typeParseResult) {
        return typeParseResult;
    }
    result.type = get<u64>(typeParseResult->consumed);
    if (result.type == 4) {
        return ParseError{ "type 4 is reserved for literal packets" };
    }
    auto lengthTypeResult = parseAnyBit()(typeParseResult->remainder);
    if (!lengthTypeResult) {
        return lengthTypeResult;
    }
    auto remainder = lengthTypeResult->remainder;
    if (get<std::string_view>(lengthTypeResult->consumed).front() == '0') {
        // length type == 0
        auto bitLengthResult = parseAnyNumber(15)(remainder);
        if (!bitLengthResult) {
            return bitLengthResult;
        }
        remainder = bitLengthResult->remainder;
        const auto bitLength = get<u64>(bitLengthResult->consumed);
        auto subPacketsStringResult = parseAnyBits(bitLength)(remainder);
        if (!subPacketsStringResult) {
            return subPacketsStringResult;
        }
        remainder = subPacketsStringResult->remainder;
        auto subPacketsBinary = get<std::string_view>(subPacketsStringResult->consumed);
        auto subPacketsResult = parseOnceOrMore(parseAnyPacket())(subPacketsBinary);
        if (!subPacketsResult) {
            return subPacketsResult;
        }
        result.subPacketsInfo = get<std::vector<ResultDescription>>(subPacketsResult->consumed);
        return ResultDescription{ std::move(result), remainder };
    } else {
        // length type == 1
        auto packetCountResult = parseAnyNumber(11)(remainder);
        if (!packetCountResult) {
            return packetCountResult;
        }
        remainder = packetCountResult->remainder;
        const auto count = get<u64>(packetCountResult->consumed);
        auto repetitionResult = parseNTimes(parseAnyPacket(), count)(remainder);
        if (!repetitionResult) {
            return repetitionResult;
        }
        result.subPacketsInfo = get<std::vector<ResultDescription>>(repetitionResult->consumed);
        return ResultDescription{ std::move(result), repetitionResult->remainder };
    }
}

[[nodiscard]] std::string decode(const std::string_view input) {
    auto binary = std::string{};
    binary.reserve(input.size() * 4);
    for (const auto c : input) {
        binary += decodeHexadecimal(c);
    }
    return binary;
}

[[nodiscard]] u64 accumulateVersionNumbers(const ParseResult& parseResult) {
    if (holds_alternative<LiteralPacket>(parseResult->consumed)) {
        return get<LiteralPacket>(parseResult->consumed).version;
    } else if (holds_alternative<OperatorPacket>(parseResult->consumed)) {
        const auto& operatorPacket = get<OperatorPacket>(parseResult->consumed);
        auto sum = operatorPacket.version;
        for (const auto& subPacket : operatorPacket.subPacketsInfo) {
            sum += accumulateVersionNumbers(subPacket);
        }
        return sum;
    } else {
        assert(false and "unexpected result type!");
        return 0;
    }
}

enum class PacketType {
    Sum,
    Product,
    Minimum,
    Maximum,
    Literal,
    GreaterThan,
    LessThan,
    EqualTo,
};

[[nodiscard]] PacketType getPacketType(const ParseResult& parseResult) {
    if (holds_alternative<LiteralPacket>(parseResult->consumed)) {
        return PacketType::Literal;
    }
    assert(holds_alternative<OperatorPacket>(parseResult->consumed));
    const auto type = get<OperatorPacket>(parseResult->consumed).type;
    switch (type) {
        case 0:
            return PacketType::Sum;
        case 1:
            return PacketType::Product;
        case 2:
            return PacketType::Minimum;
        case 3:
            return PacketType::Maximum;
        case 5:
            return PacketType::GreaterThan;
        case 6:
            return PacketType::LessThan;
        case 7:
            return PacketType::EqualTo;
        default:
            assert(false && "invalid type");
            return PacketType::Sum;
    }
}

[[nodiscard]] u64 evaluate(const ParseResult& parseResult) {
    const auto type = getPacketType(parseResult);
    if (type == PacketType::Literal) {
        return get<LiteralPacket>(parseResult->consumed).value;
    }
    const auto& subPackets = get<OperatorPacket>(parseResult->consumed).subPacketsInfo;
    switch (type) {
        case PacketType::Sum: {
            auto sum = u64{};
            for (const auto& subPacket : subPackets) {
                sum += evaluate(subPacket);
            }
            return sum;
        }
        case PacketType::Product: {
            auto product = u64{ 1 };
            for (const auto& subPacket : subPackets) {
                product *= evaluate(subPacket);
            }
            return product;
        }
        case PacketType::Minimum:
            return evaluate(*std::ranges::min_element(subPackets, std::less{}, evaluate));
        case PacketType::Maximum:
            return evaluate(*std::ranges::max_element(subPackets, std::less{}, evaluate));
        case PacketType::GreaterThan:
            return static_cast<u64>(evaluate(subPackets.at(0)) > evaluate(subPackets.at(1)));
        case PacketType::LessThan:
            return static_cast<u64>(evaluate(subPackets.at(0)) < evaluate(subPackets.at(1)));
        case PacketType::EqualTo:
            return static_cast<u64>(evaluate(subPackets.at(0)) == evaluate(subPackets.at(1)));
        default:
            assert(false && "should be unreachable");
            return 0;
    }
}

void runTestcases() {
    auto binary = decode("D2FE28");
    auto result = parseBit(true)(binary);
    assert(result);
    assert(get<std::string_view>(result->consumed) == "1");
    assert(result->remainder == "10100101111111000101000");
    result = parseBit(false)(binary);
    assert(!result);
    result = parseAnyBits(3)(binary);
    assert(result);
    assert(get<std::string_view>(result->consumed) == "110");
    assert(result->remainder == "100101111111000101000");
    result = parseAnyBits(24)(binary);
    assert(result);
    result = parseAnyBits(25)(binary);
    assert(!result);
    result = parseAnyNumber(3)(binary);
    assert(result);
    assert(get<u64>(result->consumed) == 6);
    assert(result->remainder == "100101111111000101000");
    result = parseNumber(6, 3)(binary);
    assert(result);
    assert(get<u64>(result->consumed) == 6);
    assert(result->remainder == "100101111111000101000");
    result = parseNumber(7, 3)(binary);
    assert(!result);

    result = parseAllOf(parseAnyNumber(3), parseAnyNumber(3))(binary);
    assert(result);
    assert(holds_alternative<std::vector<ResultDescription>>(result->consumed));
    assert(get<u64>(get<std::vector<ResultDescription>>(result->consumed).front().consumed) == 6);
    assert(get<u64>(get<std::vector<ResultDescription>>(result->consumed).at(1).consumed) == 4);

    result = parseAnyOf(parseNumber(6, 3), parseNumber(7, 3))(binary);
    assert(result);
    assert(get<u64>(result->consumed) == 6);
    assert(result->remainder == "100101111111000101000");

    result = parseAnyOf(parseNumber(7, 3), parseNumber(6, 3))(binary);
    assert(result);
    assert(get<u64>(result->consumed) == 6);
    assert(result->remainder == "100101111111000101000");

    result = parseAnyOf(parseNumber(5, 3), parseNumber(7, 3))(binary);
    assert(!result);

    result = parseAnyLiteralPacket()(binary);
    assert(result);
    const auto& literalPacket = get<LiteralPacket>(result->consumed);
    assert(literalPacket.version == 6);
    assert(literalPacket.type == 4);
    assert(literalPacket.value == 2021);

    result = parseAnyPacket()(binary);
    assert(result);
    assert(holds_alternative<LiteralPacket>(result->consumed));
    assert(get<LiteralPacket>(result->consumed).version == 6);
    assert(get<LiteralPacket>(result->consumed).type == 4);
    assert(get<LiteralPacket>(result->consumed).value == 2021);

    binary = decode("38006F45291200");
    result = ParseAnyOperatorPacket{}(binary);
    assert(result);
    assert(holds_alternative<OperatorPacket>(result->consumed));
    assert(get<OperatorPacket>(result->consumed).subPacketsInfo.size() == 2);
    assert(holds_alternative<LiteralPacket>(get<OperatorPacket>(result->consumed).subPacketsInfo.at(0).consumed));
    assert(get<LiteralPacket>(get<OperatorPacket>(result->consumed).subPacketsInfo.at(0).consumed).value == 10);
    assert(holds_alternative<LiteralPacket>(get<OperatorPacket>(result->consumed).subPacketsInfo.at(1).consumed));
    assert(get<LiteralPacket>(get<OperatorPacket>(result->consumed).subPacketsInfo.at(1).consumed).value == 20);

    binary = decode("38006F45291200");
    result = parseAnyPacket()(binary);
    assert(result);
    assert(holds_alternative<OperatorPacket>(result->consumed));
    assert(get<OperatorPacket>(result->consumed).subPacketsInfo.size() == 2);
    assert(holds_alternative<LiteralPacket>(get<OperatorPacket>(result->consumed).subPacketsInfo.at(0).consumed));
    assert(get<LiteralPacket>(get<OperatorPacket>(result->consumed).subPacketsInfo.at(0).consumed).value == 10);
    assert(holds_alternative<LiteralPacket>(get<OperatorPacket>(result->consumed).subPacketsInfo.at(1).consumed));
    assert(get<LiteralPacket>(get<OperatorPacket>(result->consumed).subPacketsInfo.at(1).consumed).value == 20);

    assert(accumulateVersionNumbers(result) == 1 + 6 + 2);

    binary = decode("EE00D40C823060");
    result = parseAnyPacket()(binary);
    assert(accumulateVersionNumbers(result) == 7 + 2 + 4 + 1);

    assert(accumulateVersionNumbers(parseAnyPacket()(decode("8A004A801A8002F478"))) == 16);
    assert(accumulateVersionNumbers(parseAnyPacket()(decode("620080001611562C8802118E34"))) == 12);
    assert(accumulateVersionNumbers(parseAnyPacket()(decode("C0015000016115A2E0802F182340"))) == 23);
    assert(accumulateVersionNumbers(parseAnyPacket()(decode("A0016C880162017C3686B18A3D4780"))) == 31);
}

void runTestcasesPart2() {
    assert(evaluate(parseAnyPacket()(decode("C200B40A82"))) == 3);
    assert(evaluate(parseAnyPacket()(decode("04005AC33890"))) == 54);
    assert(evaluate(parseAnyPacket()(decode("880086C3E88112"))) == 7);
    assert(evaluate(parseAnyPacket()(decode("CE00C43D881120"))) == 9);
    assert(evaluate(parseAnyPacket()(decode("D8005AC2A8F0"))) == 1);
    assert(evaluate(parseAnyPacket()(decode("F600BC2D8F"))) == 0);
    assert(evaluate(parseAnyPacket()(decode("9C005AC2F8F0"))) == 0);
    assert(evaluate(parseAnyPacket()(decode("9C0141080250320F1802104A08"))) == 1);
}

int main() {
    runTestcases();

    const auto binary = decode(readInput("input.txt").front());
    const auto result = parseAnyPacket()(binary);
    std::cout << accumulateVersionNumbers(result) << "\n";

    runTestcasesPart2();

    std::cout << evaluate(parseAnyPacket()(decode(readInput("input.txt").front()))) << "\n";
}
