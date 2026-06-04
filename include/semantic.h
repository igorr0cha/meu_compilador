#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <string>
#include <unordered_map>
#include <vector>
#include "lexer.h"

class SemanticAnalyzer {
public:
    enum TypeCode {
        TYPE_ERROR = 0,
        TYPE_INT,
        TYPE_REAL,
        TYPE_CHAR,
        TYPE_BOOL,
        TYPE_STR
    };

    struct Symbol {
        std::string name;
        TypeCode type;
        bool isArray;
        int line;
        int col;
    };

private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    Lexer* lexer;

public:
    explicit SemanticAnalyzer(Lexer* l);
    void enterScope();
    void exitScope();
    void declare(const std::string& name, TypeCode type, bool isArray, int line, int col);
    Symbol* lookup(const std::string& name);
    void checkAssignment(const std::string& name, TypeCode exprType, bool isArrayAccess);
    TypeCode tokenTagToType(int tag) const;
    void error(const std::string& msg) const;
};

#endif // SEMANTIC_H
