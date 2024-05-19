// define an OR combinator function that can combine the variableParser and plusParser functions to make a combinator to replace the variableOrPlusParser function

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

// OR combinator to combine two parsers
Parser orParser(const Parser& parser1, const Parser& parser2) {
    return [parser1, parser2](const string& input) -> optional<string> {
        // Try parsing using the first parser
        optional<string> result = parser1(input);
        if (result) {
            return result;
        }

        // Try parsing using the second parser
        return parser2(input);
    };
}

// Test the parser
int main() {
    string input = "abc + 123 + x + y_2";

    // Define parsers for variable names and the '+' operator
    Parser variableParser = [](const string& input) -> optional<string> {
        if (!input.empty() && isalpha(input[0])) {
            // Try to parse a variable name
            size_t length = 1;
            while (length < input.size() && (isalnum(input[length]) || input[length] == '_')) {
                length++;
            }
            return input.substr(0, length);
        } else {
            return nullopt;
        }
    };

    Parser plusParser = charParser('+');

    // Combine parsers using the OR combinator
    auto parser = orParser(variableParser, plusParser);

    // Parse variable names or '+' operators
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


