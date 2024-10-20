#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <unordered_set>
#include <utility>
#include <vector>
#include <iomanip>

using namespace std;

enum TokenType {
    KEYWORD,
    IDENTIFIER,

    INTEGER_LITERAL,
    LONG_LITERAL,
    FLOAT_LITERAL,
    DOUBLE_LITERAL,

    STRING_LITERAL,
    CHAR_LITERAL,

    BOOLEAN_LITERAL,
    NULL_LITERAL,

    OPERATOR,
    SEPARATOR,
    COMMENT,
    UNKNOWN
};

std::string getTokenTypeName(const TokenType type) {
    switch (type) {
        case KEYWORD: return "KEYWORD";
        case IDENTIFIER: return "IDENTIFIER";
        case INTEGER_LITERAL: return "INTEGER_LITERAL";
        case LONG_LITERAL: return "LONG_LITERAL";
        case FLOAT_LITERAL: return "FLOAT_LITERAL";
        case DOUBLE_LITERAL: return "DOUBLE_LITERAL";
        case STRING_LITERAL: return "STRING_LITERAL";
        case CHAR_LITERAL: return "CHAR_LITERAL";
        case BOOLEAN_LITERAL: return "BOOLEAN_LITERAL";
        case NULL_LITERAL: return "NULL_LITERAL";
        case OPERATOR: return "OPERATOR";
        case SEPARATOR: return "SEPARATOR";
        case COMMENT: return "COMMENT";
        case UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

struct Token {
    std::string value;
    TokenType type;

    Token(string v, const TokenType t) : value(std::move(v)), type(t) {}
};

unordered_set<string> reservedKeywords = {
        "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char",
        "class", "continue", "const", "default", "do", "double", "else", "enum",
        "exports", "extends", "final", "finally", "float", "for", "goto", "if",
        "implements", "import", "instanceof", "int", "interface", "long", "module", "native", "new",
        "package", "private", "protected", "public", "requires", "return", "short", "static", "strictfp",
        "super", "switch", "synchronized", "this", "throw", "throws", "transient", "try",
        "var", "void", "volatile", "while",
    };

unordered_set<string> operators = {
    "+", "-", "*", "/", "=", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!", "++", "--"
};

unordered_set<string> separators = {
    "(", ")", "{", "}", "[", "]", ";", ",", "."
};

unordered_set<string> booleanLiterals = { "true", "false" };

unordered_set<string> nullLiteral = { "null" };

TokenType classifyComment(const std::string& token) {
    if (token.substr(0, 2) == "//") {
        return COMMENT;
    }

    if (token.substr(0, 2) == "/*" && token.substr(token.size() - 2) == "*/") {
        return COMMENT;
    }

    return UNKNOWN;
}

TokenType classifyKeyword(const std::string& token) {
    if (reservedKeywords.contains(token)) {
        return KEYWORD;
    }
    return UNKNOWN;
}

TokenType classifySeparator(const std::string& token) {
    if (separators.contains(token)) {
        return SEPARATOR;
    }
    return UNKNOWN;
}

TokenType classifyOperator(const std::string& token) {
    if (operators.contains(token)) {
        return OPERATOR;
    }
    return UNKNOWN;
}

TokenType classifyNumericLiteral(const std::string& token) {
    if (token.back() == 'L' || token.back() == 'l') {
        return LONG_LITERAL;
    }
    if (token.back() == 'F' || token.back() == 'f') {
        return FLOAT_LITERAL;
    }
    if (token.find('.') != std::string::npos) {
        return DOUBLE_LITERAL;
    }
    return INTEGER_LITERAL;
}

TokenType classifyLiteral(const std::string& token) {
    if (token.size() >= 2 && token.front() == '"' && token.back() == '"') {
        return STRING_LITERAL;
    }
    if (token.size() == 3 && token.front() == '\'' && token.back() == '\'') {
        return CHAR_LITERAL;
    }
    if (token.size() == 4 && token.front() == '\'' && token.back() == '\'' && token[1] == '\\') {
        return CHAR_LITERAL;
    }
    return UNKNOWN;
}

TokenType classifyBooleanLiteral(const std::string& token) {
    if (booleanLiterals.contains(token)) {
        return BOOLEAN_LITERAL;
    }
    return UNKNOWN;
}

TokenType classifyNullLiteral(const std::string& token) {
    if (token == "null") {
        return NULL_LITERAL;
    }
    return UNKNOWN;
}

TokenType classifyToken(const std::string& token) {
    if (const TokenType commentType = classifyComment(token); commentType == COMMENT) {
        return COMMENT;
    }

    if (const TokenType separatorType = classifySeparator(token); separatorType == SEPARATOR) {
        return SEPARATOR;
    }

    if (const TokenType operatorType = classifyOperator(token); operatorType == OPERATOR) {
        return OPERATOR;
    }

    if (const TokenType stringLiteralType = classifyLiteral(token); stringLiteralType == STRING_LITERAL) {
        return STRING_LITERAL;
    }

    if (const TokenType charLiteralType = classifyLiteral(token); charLiteralType == CHAR_LITERAL) {
        return CHAR_LITERAL;
    }

    if (const TokenType booleanLiteralType = classifyBooleanLiteral(token); booleanLiteralType == BOOLEAN_LITERAL) {
        return BOOLEAN_LITERAL;
    }

    if (const TokenType nullLiteralType = classifyNullLiteral(token); nullLiteralType == NULL_LITERAL) {
        return NULL_LITERAL;
    }

    if (isdigit(token.front())) {
        return classifyNumericLiteral(token);
    }

    if (const TokenType keywordType = classifyKeyword(token); keywordType == KEYWORD) {
        return KEYWORD;
    }

    if (isalpha(token.front()) || token.front() == '_') {
        return IDENTIFIER;
    }

    return UNKNOWN;
}

vector<string> splitIntoTokens(const string& line) {
    vector<string> tokens;
    string currentToken;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        if (isspace(c)) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            continue;
        }

        if (separators.contains(string(1, c))) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            tokens.emplace_back(1, c);
            continue;
        }

        if (i < line.size() - 1) {
            string twoCharOperator = string(1, c) + line[i + 1];
            if (operators.contains(twoCharOperator)) {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.push_back(twoCharOperator);
                ++i;
                continue;
            }
        }

        if (operators.contains(string(1, c))) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            tokens.emplace_back(1, c);
            continue;
        }

        if (isdigit(c)) {
            currentToken += c;
            if (i + 1 < line.size() && line[i + 1] == '.' && i + 2 < line.size() && isdigit(line[i + 2])) {
                currentToken += line[i + 1];
                ++i;
                while (i + 1 < line.size() && isdigit(line[i + 1])) {
                    currentToken += line[++i];
                }
            }
            continue;
        }

        if (c == '"') {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            currentToken += c;
            ++i;
            while (i < line.size() && line[i] != '"') {
                currentToken += line[i++];
            }
            currentToken += '"';
            tokens.push_back(currentToken);
            currentToken.clear();
            continue;
        }

        if (isalpha(c) || c == '_') {
            currentToken += c;
            while (i + 1 < line.size() && (isalnum(line[i + 1]) || line[i + 1] == '.' || line[i + 1] == '_')) {
                currentToken += line[++i];
            }
            tokens.push_back(currentToken);
            currentToken.clear();
            continue;
        }

        currentToken += c;
    }

    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }

    return tokens;
}

void printFormattedToken(const std::string& token, TokenType type) {
    cout << left << setw(20) << token;
    cout << " | " << getTokenTypeName(type) << endl;
}

int main() {
    ifstream file("test.txt");
    string line;

    if (!file) {
        cerr << "Error: Unable to open file 'test.txt'" << endl;
        return 1;
    }

    while (getline(file, line)) {
        for (vector<string> tokens = splitIntoTokens(line); const string& token : tokens) {
            const TokenType type = classifyToken(token);
            printFormattedToken(token, type);
        }
    }

    file.close();
    return 0;
}
