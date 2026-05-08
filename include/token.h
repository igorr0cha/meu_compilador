#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include "tags.h"

/**
 * Hierarquia de classes para representar diferentes tipos de tokens.
 * 
 * Single Responsibility Principle: Cada classe é responsável apenas por
 * armazenar os dados específicos de seu tipo de token.
 */

class Token {
public:
    int tag;
    
    explicit Token(int t) : tag(t) {}
    virtual ~Token() = default;
    
    virtual std::string lexeme() const { return std::string(1, (char)tag); }
};

/**
 * Token que representa um número inteiro.
 */
class NumInt : public Token {
public:
    int value;
    
    explicit NumInt(int v) : Token(NUM_INT), value(v) {}
    
    std::string lexeme() const override {
        return std::to_string(value);
    }
};

/**
 * Token que representa um número real.
 */
class NumReal : public Token {
public:
    double value;
    
    explicit NumReal(double v) : Token(NUM_REAL), value(v) {}
    
    std::string lexeme() const override {
        return std::to_string(value);
    }
};

/**
 * Token que representa uma palavra (identificador, string literal, ou palavra-chave).
 */
class Word : public Token {
public:
    std::string lexeme_str;
    
    Word(const std::string& s, int t) : Token(t), lexeme_str(s) {}
    
    std::string lexeme() const override {
        return lexeme_str;
    }
};

#endif // TOKEN_H
