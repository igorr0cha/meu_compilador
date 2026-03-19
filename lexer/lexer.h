#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cctype>
#include <cstdlib> 
#include "tags.h"

using namespace std;

class Token {
public:
    int tag;
    Token(int t) : tag(t) {}
    virtual ~Token() {} 
};

class NumInt : public Token {
public:
    int value;
    NumInt(int v) : Token(NUM_INT), value(v) {}
};

class NumReal : public Token {
public:
    double value;
    NumReal(double v) : Token(NUM_REAL), value(v) {}
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
    ifstream file;
    int line;
    int col;

    void reserve(Word* w) {
        words[w->lexeme] = w;
    }

    void read() {
        peek = file.get();
        if (peek == '\n') {
            line++;
            col = 0; 
        } else {
            col++;
        }
    }

    bool readch(char c) {
        read();
        if (peek != c) return false;
        peek = ' ';
        return true;
    }

public:
    Lexer(const string& filename) : peek(' '), line(1), col(0) {
        file.open(filename.c_str()); 
        if (!file.is_open()) {
            cerr << "Erro: Nao foi possivel abrir o arquivo " << filename << endl;
            exit(1);
        }

        // Populando a tabela com palavras reservadas
        reserve(new Word("algoritmo", ALGORITMO)); reserve(new Word("inicio", INICIO)); reserve(new Word("fim", FIM));
        reserve(new Word("se", SE)); reserve(new Word("entao", ENTAO)); reserve(new Word("senao", SENAO));
        reserve(new Word("enquanto", ENQUANTO)); reserve(new Word("faca", FACA));
        reserve(new Word("para", PARA)); reserve(new Word("de", DE)); reserve(new Word("ate", ATE));
        reserve(new Word("inteiro", INTEIRO)); reserve(new Word("real", REAL));
        reserve(new Word("caractere", CARACTERE)); reserve(new Word("logico", LOGICO));
        reserve(new Word("escreva", ESCREVA)); reserve(new Word("leia", LEIA));
    }

    ~Lexer() {
        if (file.is_open()) file.close();
        for (map<string, Word*>::iterator it = words.begin(); it != words.end(); ++it) {
            delete it->second;
        }
    }

    void printSymbolTable() {
        cout << "\n=== Tabela de Simbolos ===" << endl;
        for (map<string, Word*>::iterator it = words.begin(); it != words.end(); ++it) {
            if (it->second->tag == ID) { 
                cout << "ID: " << it->first << endl;
            }
        }
        cout << "==========================" << endl;
    }

    Token* scan() {
        for (;; read()) {
            if (peek == ' ' || peek == '\t' || peek == '\r') continue;
            else if (peek == '\n') continue;
            else break;
        }

        if (file.eof() || peek == (char)-1) return NULL;

        // --- Tratamento de Comentrios ---
        if (peek == '/') {
            read();
            if (peek == '/') { 
                while (peek != '\n' && !file.eof()) read();
                return scan();
            } else if (peek == '*') { 
                read();
                while (!file.eof()) {
                    if (peek == '*') {
                        read();
                        if (peek == '/') {
                            peek = ' '; 
                            return scan();
                        }
                    } else {
                        read();
                    }
                }
            } else {
                return new Token('/');
            }
        }

        // --- Lookahead para Operadores ---
        switch (peek) {
            case '<':
                read();
                if (peek == '-') { peek = ' '; return new Word("<-", ATRIBUICAO); }
                if (peek == '>') { peek = ' '; return new Word("<>", DIFERENTE); }
                if (peek == '=') { peek = ' '; return new Word("<=", MENOR_IGUAL); }
                return new Token('<');
            case '>':
                if (readch('=')) return new Word(">=", MAIOR_IGUAL);
                else return new Token('>');
        }

        // --- Reconhecimento de Strings ---
        if (peek == '"') {
            string s = "";
            read(); 
            while (peek != '"' && !file.eof()) {
                s += peek;
                read();
            }
            if (file.eof()) {
                cerr << "ERRO LXICO: String nao fechada antes do fim do ficheiro na Linha " << line << endl;
            } else {
                peek = ' '; 
            }
            return new Word(s, LITERAL_STRING);
        }

        // --- Reconhecimento de Nmeros ---
        // BLINDADO COM UNSIGNED CHAR
        if (isdigit((unsigned char)peek)) {
            int v = 0;
            do {
                v = v * 10 + (peek - '0');
                read();
            } while (isdigit((unsigned char)peek));
            
            if (peek != '.') return new NumInt(v);
            
            double x = v;
            double d = 10;
            for (;;) {
                read();
                if (!isdigit((unsigned char)peek)) break;
                x = x + (peek - '0') / d;
                d = d * 10;
            }
            return new NumReal(x);
        }

        // --- Reconhecimento de Identificadores ---
        // BLINDADO COM UNSIGNED CHAR
        if (isalpha((unsigned char)peek)) {
            string s = "";
            do {
                s += peek;
                read();
            } while (isalnum((unsigned char)peek) || peek == '_'); 

            if (words.count(s)) return words[s];

            Word* w = new Word(s, ID);
            reserve(w); 
            return w;
        }

        // --- Caracteres Desconhecidos e BOM do txt ---
        string valid_chars = "()[],:;+-*%=_";
        // Ignora silenciosamente caracteres de quebra especiais negativos como o BOM do UTF-8
        if (peek < 0) {
            peek = ' ';
            return scan();
        }
        
        if (valid_chars.find(peek) == string::npos) {
            cerr << "ERRO LEXICO: Simbolo nao reconhecido '" << peek 
                 << "' na Linha " << line << ", Coluna " << col << endl;
            peek = ' ';
            return scan();
        }

        Token* t = new Token(peek);
        peek = ' '; 
        return t;
    }
};

#endif
