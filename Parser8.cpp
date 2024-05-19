// How can the code be modified to identify binary operators such as +  in the expression and return an object for evaluation ?

#include <iostream>
#include <string>
#include <optional>
#include <functional>
#include <cctype>
#include <memory>
#include <variant>

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
            return make_pair(make_unique<BinaryOpNode>(c, nullptr, nullptr), input.substr(1));
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

// Define parsers for variable names and the '+' operator
Parser variableParser = [](const string& input) -> optional<pair<unique_ptr<ASTNode>, string>> {
    if (!input.empty() && isalpha(input[0])) {
        // Try to parse a variable name
        size_t length = 1;
        while (length < input.size() && (isalnum(input[length]) || input[length] == '_')) {
            length++;
        }
        string name = input.substr(0, length);
        return make_pair(make_unique<VariableNode>(name), input.substr(length));
    } else {
        return nullopt;
    }
};

Parser plusParser = charParser('+');

// Combine parsers using the OR combinator
auto parser = orParser(variableParser, plusParser);

// Test the parser
int main() {
    string input = "abc + 123 + x + y_2";

    // Parse variable names or '+' operators
    size_t startPos = 0;
    while (startPos < input.size()) {
        if (auto result = parser(input.substr(startPos)); result) {
            unique_ptr<ASTNode> node = move(result->first);
            string remaining = result->second;
            cout << "Parsed: ";
            node->print();
            cout << endl;
            startPos += input.size() - remaining.size();
        } else {
            // Skip invalid characters
            startPos++;
        }
    }

    return 0;
}


