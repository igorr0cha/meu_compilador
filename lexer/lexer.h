#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <string>
#include <map>
#include <cctype>
#include "tags.h"

using namespace std;

class Token {
public:
    int tag;
    Token(int t) : tag(t) {}
    virtual ~Token() {} 
};

class Num : public Token {
public:
    int value;
    Num(int v) : Token(NUM), value(v) {}
};

class Word : public Token {
public:
    string lexeme;
    Word(string s, int t) : Token(t), lexeme(s) {}
};

class Lexer {
private:
    char peek;
    map<string, Word*> words; 

    void reserve(Word* w) {
        words[w->lexeme] = w;
    }

    char read() {
        return cin.get();
    }

public:
    Lexer() : peek(' ') { 
        // Populando a tabela de símbolos com as palavras em português
        reserve(new Word("verdadeiro", VERDADEIRO));
        reserve(new Word("falso", FALSO));
        reserve(new Word("se", SE));
        reserve(new Word("senao", SENAO));
        reserve(new Word("enquanto", ENQUANTO));
        reserve(new Word("para", PARA));
        reserve(new Word("inteiro", INTEIRO));
        reserve(new Word("real", REAL));
        reserve(new Word("imprimir", IMPRIMIR));
    }

    Token* scan() {
        while (peek == ' ' || peek == '\t' || peek == '\n' || peek == '\r') {
            peek = read();
        }

        if (cin.eof() || peek == (char)-1) return NULL;

        // Reconhecimento de Números
        if (isdigit(peek)) {
            int v = 0;
            do {
                v = v * 10 + (peek - '0');
                peek = read();
            } while (isdigit(peek));
            return new Num(v);
        }

        // Reconhecimento de Identificadores e Palavras Reservadas
        if (isalpha(peek)) {
            string s = "";
            do {
                s += peek;
                peek = read();
            } while (isalnum(peek));

            if (words.count(s)) return words[s];

            Word* w = new Word(s, ID);
            words[s] = w;
            return w;
        }

        // Operadores e outros caracteres
        Token* t = new Token(peek);
        peek = ' '; 
        return t;
    }
};

#endif
