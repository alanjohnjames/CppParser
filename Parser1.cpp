// write a c++ parser combinator function to parse a char



#include <iostream>
#include <string>
#include <optional>
#include <functional>

using namespace std;

// Define a parser combinator function type
using Parser = function<optional<char>(const string&)>;

// Parser combinator function to parse a single character
Parser charParser(char c) {
    return [c](const string& input) -> optional<char> {
        if (!input.empty() && input[0] == c) {
            return c;
        } else {
            return nullopt;
        }
    };
}

// Test the parser
int main() {
    string input = "abc";

    // Parse 'a'
    auto parserA = charParser('a');
    optional<char> resultA = parserA(input);
    if (resultA) {
        cout << "Parsed character: " << resultA.value() << endl;
    } else {
        cout << "Failed to parse character 'a'" << endl;
    }

    // Parse 'x'
    auto parserX = charParser('x');
    optional<char> resultX = parserX(input);
    if (resultX) {
        cout << "Parsed character: " << resultX.value() << endl;
    } else {
        cout << "Failed to parse character 'x'" << endl;
    }

    return 0;
}


