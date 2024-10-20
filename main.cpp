#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <unordered_set>
#include <vector>

using namespace std;

enum class TokenType {
    Number,
    StringLiteral,
    Comment,
    ReservedWord,
    Operator,
    Punctuation,
    Identifier,
    Error
};

struct Token {
    string lexeme;
    TokenType type;
};

unordered_set<string> reservedWords = {
    "break", "case", "catch", "class", "const", "continue", "debugger", "default",
    "delete", "do", "else", "export", "extends", "finally", "for", "function",
    "if", "import", "in", "instanceof", "new", "return", "super", "switch",
    "this", "throw", "try", "typeof", "var", "void", "while", "with", "yield",
    // ES6
    "let", "static", "enum", "await"
};

unordered_set<string> operators = {
    "+", "-", "*", "/", "%", "++", "--", "==", "!=", "===", "!==", ">", "<", ">=", "<=",
    "&&", "||", "!", "&", "|", "^", "~", "<<", ">>", ">>>", "?", ":", "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=", ">>>="
};

unordered_set<char> punctuation = {
    '(', ')', '{', '}', '[', ']', ';', ',', '.', ':'
};

bool isIdentifierStart(char c) {
    return isalpha(c) || c == '_' || c == '$';
}

bool isIdentifierPart(char c) {
    return isalnum(c) || c == '_' || c == '$';
}

vector<Token> tokenize(const string& code) {
    vector<Token> tokens;
    size_t i = 0;
    size_t length = code.length();

    while (i < length) {
        char c = code[i];

        if (isspace(c)) {
            i++;
            continue;
        }

        if (c == '/') {
            if (i + 1 < length) {
                if (code[i + 1] == '/') {
                    size_t start = i;
                    i += 2;
                    while (i < length && code[i] != '\n') {
                        i++;
                    }
                    tokens.push_back({code.substr(start, i - start), TokenType::Comment});
                    continue;
                } else if (code[i + 1] == '*') {
                    size_t start = i;
                    i += 2;
                    while (i + 1 < length && !(code[i] == '*' && code[i + 1] == '/')) {
                        i++;
                    }
                    if (i + 1 >= length) {
                        tokens.push_back({code.substr(start), TokenType::Error});
                        break;
                    }
                    i += 2;
                    tokens.push_back({code.substr(start, i - start), TokenType::Comment});
                    continue;
                }
            }
        }

        if (c == '"' || c == '\'') {
            char quoteType = c;
            size_t start = i;
            i++;
            while (i < length && code[i] != quoteType) {
                if (code[i] == '\\') {
                    i++;
                }
                i++;
            }
            if (i >= length) {
                tokens.push_back({code.substr(start), TokenType::Error});
                break;
            }
            i++;
            tokens.push_back({code.substr(start, i - start), TokenType::StringLiteral});
            continue;
        }

        if (isdigit(c) || (c == '.' && i + 1 < length && isdigit(code[i + 1]))) {
            size_t start = i;
            bool isHex = false;
            if (c == '0' && i + 1 < length && (code[i + 1] == 'x' || code[i + 1] == 'X')) {
                isHex = true;
                i += 2;
                while (i < length && isxdigit(code[i])) {
                    i++;
                }
            } else {
                while (i < length && isdigit(code[i])) {
                    i++;
                }
                if (i < length && code[i] == '.') {
                    i++;
                    while (i < length && isdigit(code[i])) {
                        i++;
                    }
                }
                if (i < length && (code[i] == 'e' || code[i] == 'E')) {
                    i++;
                    if (i < length && (code[i] == '+' || code[i] == '-')) {
                        i++;
                    }
                    while (i < length && isdigit(code[i])) {
                        i++;
                    }
                }
            }
            tokens.push_back({code.substr(start, i - start), TokenType::Number});
            continue;
        }

        if (isIdentifierStart(c)) {
            size_t start = i;
            i++;
            while (i < length && isIdentifierPart(code[i])) {
                i++;
            }
            string lexeme = code.substr(start, i - start);
            if (reservedWords.count(lexeme)) {
                tokens.push_back({lexeme, TokenType::ReservedWord});
            } else {
                tokens.push_back({lexeme, TokenType::Identifier});
            }
            continue;
        }

        bool matchedOperator = false;
        for (size_t opLen = 3; opLen > 0; opLen--) {
            if (i + opLen <= length) {
                string op = code.substr(i, opLen);
                if (operators.count(op)) {
                    tokens.push_back({op, TokenType::Operator});
                    i += opLen;
                    matchedOperator = true;
                    break;
                }
            }
        }
        if (matchedOperator) {
            continue;
        }

        if (punctuation.count(c)) {
            tokens.push_back({string(1, c), TokenType::Punctuation});
            i++;
            continue;
        }

        tokens.push_back({string(1, c), TokenType::Error});
        i++;
    }

    return tokens;
}

string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::Number: return "Number";
        case TokenType::StringLiteral: return "StringLiteral";
        case TokenType::Comment: return "Comment";
        case TokenType::ReservedWord: return "ReservedWord";
        case TokenType::Operator: return "Operator";
        case TokenType::Punctuation: return "Punctuation";
        case TokenType::Identifier: return "Identifier";
        case TokenType::Error: return "Error";
        default: return "Unknown";
    }
}

int main() {
    string code = R"(

    // JavaScript
    var x = 42;
    function greet(name) {
        console.log("Hello, " + name);
    }

    )";

    vector<Token> tokens = tokenize(code);

    for (const auto& token : tokens) {
        cout << "< " << token.lexeme << " , " << tokenTypeToString(token.type) << " >" << endl;
    }

    return 0;
}