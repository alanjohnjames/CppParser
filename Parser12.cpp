// under what circumstances would the andParser be used?  show an example


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

class NumberNode : public ASTNode {
public:
    int value;
    NumberNode(int value) : value(value) {}
    void print() const override {
        cout << "Number(" << value << ")";
    }
};

class AssignmentNode : public ASTNode {
public:
    unique_ptr<ASTNode> left;
    unique_ptr<ASTNode> right;
    AssignmentNode(unique_ptr<ASTNode> left, unique_ptr<ASTNode> right)
        : left(move(left)), right(move(right)) {}
    void print() const override {
        cout << "Assignment(";
        left->print();
        cout << " = ";
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

    // Use the combined alnum and underscore parser to parse the variable name
    while (pos < input.size()) {
        auto parser = orParser(alnumParser, underscoreParser);
        if (auto result = parser(input.substr(pos)); result) {
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

// Parser for digits
Parser digitParser = [](const string& input) -> optional<pair<unique_ptr<ASTNode>, string>> {
    if (!input.empty() && isdigit(input[0])) {
        int value = input[0] - '0';
        return make_pair(make_unique<NumberNode>(value), input.substr(1));
    } else {
        return nullopt;
    }
};

// Parser for numbers (sequence of digits)
Parser numberParser = [](const string& input) -> optional<pair<unique_ptr<ASTNode>, string>> {
    string numberStr;
    size_t pos = 0;
    while (pos < input.size() && isdigit(input[pos])) {
        numberStr.push_back(input[pos]);
        pos++;
    }
    if (!numberStr.empty()) {
        return make_pair(make_unique<NumberNode>(stoi(numberStr)), input.substr(pos));
    } else {
        return nullopt;
    }
};

// Parser for the '=' operator
Parser equalsParser = charParser('=');

// Parser for assignment (variable = number)
Parser assignmentParser = [](const string& input) -> optional<pair<unique_ptr<ASTNode>, string>> {
    auto variableResult = variableParser(input);
    if (!variableResult) return nullopt;

    auto equalsResult = equalsParser(variableResult->second);
    if (!equalsResult) return nullopt;

    auto numberResult = numberParser(equalsResult->second);
    if (!numberResult) return nullopt;

    return make_pair(
        make_unique<AssignmentNode>(move(variableResult->first), move(numberResult->first)),
        numberResult->second
    );
};

// Combine parsers using the OR combinator
auto combinedParser = orParser(assignmentParser, variableParser, plusParser);

// Test the parser
int main() {
    string input = "x = 42 + y_2";

    // Parse the input
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


