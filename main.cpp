#include <iostream>
#include <regex>
#include <fstream>
#include <vector>

using namespace std;

vector<pair<regex, string>> setupPatterns() {
    return {
        {regex("(\\s+)"), "Whitespace"},
        {regex("\\b(abstract|assert|boolean|break|byte|case|catch|char|class|continue|default|do|double|else|enum|exports|extends|final|finally|float|for|goto|if|implements|import|instanceof|int|interface|long|module|native|new|package|private|protected|public|requires|return|short|static|strictfp|super|switch|synchronized|this|throw|throws|transient|try|var|void|volatile|while)\\b"), "Keyword"},
        {regex("\\b(true|false|null)\\b"), "Literal"},
        {regex("\\b([a-zA-Z_][a-zA-Z0-9_]*)\\b"), "Identifier"},
        {regex(R"(\b\d+\.\d+([eE][+-]?\d+)?[fFdD]?\b)"), "Float Literal"},
        {regex(R"(\b\d+[lL]?\b)"), "Integer Literal"},
        {regex(R"("(\\.|[^"])*")"), "String Literal"},
        {regex("'(\\\\.|[^'])'"), "Char Literal"},
        {regex(R"((//.*$|/\*[\s\S]*?\*/))", regex::multiline), "Comment"},
        {regex(R"((\+\+|--|\+=|-=|\*=|/=|%=|==|!=|<=|>=|&&|\|\||\+|-|\*|/|%|=|>|<|!|~|&|\||\^|<<|>>|>>>|\?|:))"), "Operator"},
        {regex(R"((\{|\}|\(|\)|\[|\]|;|,|\.))"), "Punctuation"},
        {regex(R"([^a-zA-Z0-9_+=\-*/%&|!^<>=~#"'().,{};\s`\[\]\$\:\?\\@])"), "Unrecognized Token"}
    };
}

void processCode(const string& sourceCode, const vector<pair<regex, string>>& tokenPatterns) {
    string codeBuffer = sourceCode;

    while (!codeBuffer.empty()) {
        smatch tokenMatch;
        bool isMatched = false;

        for (const auto& token : tokenPatterns) {
            if (regex_search(codeBuffer, tokenMatch, token.first)) {
                if (tokenMatch.position() == 0) {
                    if (token.second != "Whitespace") {
                        cout << tokenMatch[0] << " - " << token.second << "\n";
                    }
                    codeBuffer = tokenMatch.suffix().str();
                    isMatched = true;
                    break;
                }
            }
        }
        if (!isMatched) {
            cerr << "Unrecognized token: " << codeBuffer << "\n";
            break;
        }
    }
}

int main() {
    ifstream javaFile("test.java");
    if (!javaFile.is_open()) {
        cerr << "Can't open file test.java\n";
        return -1;
    }

    const string fileContent((istreambuf_iterator<char>(javaFile)), istreambuf_iterator<char>());

    const auto tokenDefinitions = setupPatterns();
    processCode(fileContent, tokenDefinitions);

    return 0;
}