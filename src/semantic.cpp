#include "../include/semantic.h"
#include "../include/tags.h"
#include <iostream>

using namespace std;

SemanticAnalyzer::SemanticAnalyzer(Lexer* l) : lexer(l) {
    enterScope();
}

void SemanticAnalyzer::enterScope() {
    scopes.emplace_back();
}

void SemanticAnalyzer::exitScope() {
    if (!scopes.empty()) scopes.pop_back();
}

void SemanticAnalyzer::declare(const std::string& name, TypeCode type, bool isArray, int line, int col) {
    if (scopes.empty()) enterScope();
    auto& current = scopes.back();
    if (current.find(name) != current.end()) {
        cerr << "[ERRO SEMANTICO] Linha " << line << ", Coluna " << col << ":\n";
        cerr << "  Identificador '" << name << "' ja declarado no mesmo escopo.\n";
        exit(1);
    }

    Symbol s; s.name = name; s.type = type; s.isArray = isArray; s.line = line; s.col = col;
    current[name] = s;
}

SemanticAnalyzer::Symbol* SemanticAnalyzer::lookup(const std::string& name) {
    for (int i = (int)scopes.size()-1; i >= 0; --i) {
        auto it = scopes[i].find(name);
        if (it != scopes[i].end()) return &it->second;
    }
    return nullptr;
}

void SemanticAnalyzer::checkAssignment(const std::string& name, TypeCode exprType, bool isArrayAccess) {
    Symbol* s = lookup(name);
    int line = (lexer) ? lexer->getLine() : 0;
    int col = (lexer) ? lexer->getCol() : 0;
    if (!s) {
        cerr << "[ERRO SEMANTICO] Linha " << line << ", Coluna " << col << ":\n";
        cerr << "  Identificador '" << name << "' nao declarado.\n";
        exit(1);
    }

    if (isArrayAccess && !s->isArray) {
        cerr << "[ERRO SEMANTICO] Linha " << line << ", Coluna " << col << ":\n";
        cerr << "  Variavel '" << name << "' nao e um array.\n";
        exit(1);
    }

    if (!isArrayAccess && s->isArray) {
        cerr << "[ERRO SEMANTICO] Linha " << line << ", Coluna " << col << ":\n";
        cerr << "  Variavel '" << name << "' e um array; indice esperado.\n";
        exit(1);
    }

    // Tipo compatibilidade: inteiro -> real permitido
    if (s->type == TYPE_REAL && exprType == TYPE_INT) return;
    // permitir string literal atribuida a caractere (aceitar como compativel)
    if (s->type == TYPE_CHAR && exprType == TYPE_STR) return;
    // permitir atribuir inteiro (0/1) a logico
    if (s->type == TYPE_BOOL && exprType == TYPE_INT) return;
    if (s->type == exprType) return;

    cerr << "[ERRO SEMANTICO] Linha " << line << ", Coluna " << col << ":\n";
    cerr << "  Incompatibilidade de tipos na atribuicao a '" << name << "'.\n";
    exit(1);
}

SemanticAnalyzer::TypeCode SemanticAnalyzer::tokenTagToType(int tag) const {
    switch (tag) {
        case NUM_INT: return TYPE_INT;
        case NUM_REAL: return TYPE_REAL;
        case VERDADEIRO:
        case FALSO: return TYPE_BOOL;
        case LITERAL_STRING: return TYPE_STR;
        case INTEIRO: return TYPE_INT;
        case REAL: return TYPE_REAL;
        case CARACTERE: return TYPE_CHAR;
        case LOGICO: return TYPE_BOOL;
        default: return TYPE_ERROR;
    }
}

void SemanticAnalyzer::error(const std::string& msg) const {
    int line = (lexer) ? lexer->getLine() : 0;
    int col = (lexer) ? lexer->getCol() : 0;
    cerr << "[ERRO SEMANTICO] Linha " << line << ", Coluna " << col << ":\n";
    cerr << "  " << msg << "\n";
    exit(1);
}
