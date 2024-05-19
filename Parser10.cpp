// the variableParser does not use the  alnumParser or the underscoreParser

#include <iostream>
#include <string>
#include <optional>
#include <functional>
#include <cctype>
#include <memory>
#include <variant>
#include <vector>

using namespace std;

// Abstract syntax tree (AST) node classes
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print() const = 0;
};

class VariableNode : public ASTNode {
public:
    string name;
    VariableNode(const string& name) : name(name) {}
    void print() const override {
        cout << "Variable(" << name << ")";
    }
};

class BinaryOpNode : public ASTNode {
public:
    char op;
    unique_ptr<ASTNode> left;
    unique_ptr<ASTNode> right;
    BinaryOpNode(char op, unique_ptr<ASTNode> left, unique_ptr<ASTNode> right)
        : op(op), left(move(left)), right(move(right)) {}
    void print() const override {
        cout << "BinaryOp(" << op << ", ";
        left->print();
        cout << ", ";
        right->print();
        cout << ")";
    }
};

// Define a parser combinator function type
using Parser = function<optional<pair<unique_ptr<ASTNode>, string>>(const string&)>;

// Parser combinator function to parse a single character
Parser charParser(char c) {
    return [c](const string& input) -> optional<pair<unique_ptr<ASTNode>, string>> {
        if (!input.empty() && input[0] == c) {
            return make_pair(nullptr, input.substr(1)); // Return nullptr as the node for char parsing
        } else {
            return nullopt;
        }
    };
}

// OR combinator to combine multiple parsers
template<typename... Parsers>
Parser orParser(Parsers... parsers) {
    auto parserList = {parsers...};
    return [parserList](const string& input) -> optional<pair<unique_ptr<ASTNode>, string>> {
        for (const auto& parser : parserList) {
            if (auto result = parser(input); result) {
                return result;
            }
        }
        return nullopt;
    };
}

// AND combinator to combine parsers sequentially
Parser andParser(Parser first, Parser second) {
    return [first, second](const string& input) -> optional<pair<unique_ptr<ASTNode>, string>> {
        if (auto firstResult = first(input); firstResult) {
            auto remainingInput = firstResult->second;
            if (auto secondResult = second(remainingInput); secondResult) {
                return make_pair(nullptr, secondResult->second); // Combine results
            }
        }
        return nullopt;
    };
}

// Define parsers for variable names using alnum and underscore characters
Parser alnumParser = [](const string& input) -> optional<pair<unique_ptr<ASTNode>, string>> {
    if (!input.empty() && isalnum(input[0])) {
        return make_pair(nullptr, input.substr(1));
    } else {
        return nullopt;
    }
};

Parser underscoreParser = charParser('_');

// Parser for variable names
Parser variableParser = [](const string& input) -> optional<pair<unique_ptr<ASTNode>, string>> {
    if (input.empty() || !isalpha(input[0])) {
        return nullopt;
    }

    string varName;
    size_t pos = 0;

    while (pos < input.size()) {
        if (auto result = orParser(alnumParser, underscoreParser)(input.substr(pos)); result) {
            varName.push_back(input[pos]);
            pos++;
        } else {
            break;
        }
    }

    return make_pair(make_unique<VariableNode>(varName), input.substr(pos));
};

// Parser for the '+' operator
Parser plusParser = charParser('+');

// Combine parsers using the OR combinator
auto combinedParser = orParser(variableParser, plusParser);

// Test the parser
int main() {
    string input = "abc + 123 + x + y_2";

    // Parse variable names or '+' operators
    size_t startPos = 0;
    while (startPos < input.size()) {
        if (auto result = combinedParser(input.substr(startPos)); result) {
            unique_ptr<ASTNode> node = move(result->first);
            string remaining = result->second;
            if (node) {
                cout << "Parsed: ";
                node->print();
                cout << endl;
            } else {
                cout << "Parsed: +" << endl; // Handle the plus operator
            }
            startPos += input.size() - remaining.size();
        } else {
            // Skip invalid characters
            startPos++;
        }
    }

    return 0;
}


