// change the example to combine the parsers for variable names and the operator + parser


#include <iostream>
#include <string>
#include <optional>
#include <functional>
#include <cctype>

using namespace std;

// Define a parser combinator function type
using Parser = function<optional<string>(const string&)>;

// Parser combinator function to parse a single character
Parser charParser(char c) {
    return [c](const string& input) -> optional<string> {
        if (!input.empty() && input[0] == c) {
            return string(1, c);
        } else {
            return nullopt;
        }
    };
}

// Parser combinator to parse variable names or the '+' operator
Parser variableOrPlusParser() {
    return [](const string& input) -> optional<string> {
        if (!input.empty() && isalpha(input[0])) {
            // Try to parse a variable name
            size_t length = 1;
            while (length < input.size() && (isalnum(input[length]) || input[length] == '_')) {
                length++;
            }
            return input.substr(0, length);
        } else if (!input.empty() && input[0] == '+') {
            // Parse the '+' operator
            return charParser('+')(input);
        } else {
            // Return nullopt if neither variable name nor '+' operator is found
            return nullopt;
        }
    };
}

// Test the parser
int main() {
    string input = "abc + 123 + x + y_2";

    // Parse variable names or '+' operators
    auto parser = variableOrPlusParser();
    size_t startPos = 0;
    while (startPos < input.size()) {
        optional<string> result = parser(input.substr(startPos));
        if (result) {
            cout << "Parsed: " << result.value() << endl;
            startPos += result.value().size();
        } else {
            // Skip invalid characters
            startPos++;
        }
    }

    return 0;
}
