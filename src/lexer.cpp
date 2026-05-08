#include "../include/lexer.h"
#include <iostream>
#include <cctype>
#include <cstdlib>

using namespace std;

Lexer::Lexer(const string& filename)
    : peek(' '), line(1), col(0) {
    file.open(filename.c_str());
    if (!file.is_open()) {
        cerr << "ERRO LEXICO: Nao foi possivel abrir o arquivo '" << filename << "'\n";
        exit(1);
    }
    
    // Populando a tabela de palavras-chave
    reserve(new Word("algoritmo", ALGORITMO));
    reserve(new Word("inicio", INICIO));
    reserve(new Word("fim", FIM));
    reserve(new Word("se", SE));
    reserve(new Word("entao", ENTAO));
    reserve(new Word("senao", SENAO));
    reserve(new Word("enquanto", ENQUANTO));
    reserve(new Word("faca", FACA));
    reserve(new Word("para", PARA));
    reserve(new Word("de", DE));
    reserve(new Word("ate", ATE));
    reserve(new Word("inteiro", INTEIRO));
    reserve(new Word("real", REAL));
    reserve(new Word("caractere", CARACTERE));
    reserve(new Word("logico", LOGICO));
    reserve(new Word("escreva", ESCREVA));
    reserve(new Word("leia", LEIA));
    reserve(new Word("verdadeiro", VERDADEIRO));
    reserve(new Word("falso", FALSO));
    reserve(new Word("true", VERDADEIRO));
    reserve(new Word("false", FALSO));
}

Lexer::~Lexer() {
    if (file.is_open()) {
        file.close();
    }
    for (auto& entry : words) {
        delete entry.second;
    }
}

void Lexer::reserve(Word* w) {
    words[w->lexeme_str] = w;
}

void Lexer::read() {
    peek = file.get();
    if (peek == '\n') {
        line++;
        col = 0;
    } else {
        col++;
    }
}

bool Lexer::readch(char c) {
    read();
    if (peek != c) {
        return false;
    }
    peek = ' ';
    return true;
}

Token* Lexer::scan() {
    // Pula espaços em branco
    for (;; read()) {
        if (peek == ' ' || peek == '\t' || peek == '\r') {
            continue;
        } else if (peek == '\n') {
            continue;
        } else {
            break;
        }
    }
    
    // Fim de arquivo
    if (file.eof() || peek == (char)-1) {
        return NULL;
    }
    
    // Tratamento de Comentários
    if (peek == '/') {
        read();
        if (peek == '/') {
            // Comentário de linha única
            while (peek != '\n' && !file.eof()) {
                read();
            }
            return scan(); // Recursivamente pega o próximo token
        } else if (peek == '*') {
            // Comentário multi-linha
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
    
    // Lookahead para Operadores Compostos
    switch (peek) {
        case '<':
            read();
            if (peek == '-') {
                peek = ' ';
                return new Word("<-", ATRIBUICAO);
            }
            if (peek == '>') {
                peek = ' ';
                return new Word("<>", DIFERENTE);
            }
            if (peek == '=') {
                peek = ' ';
                return new Word("<=", MENOR_IGUAL);
            }
            return new Token('<');
            
        case '>':
            if (readch('=')) {
                return new Word(">=", MAIOR_IGUAL);
            }
            return new Token('>');
    }
    
    // Reconhecimento de Strings (entre aspas duplas)
    if (peek == '"') {
        string s = "";
        read();
        while (peek != '"' && !file.eof()) {
            s += peek;
            read();
        }
        if (file.eof()) {
            cerr << "ERRO LEXICO: String nao fechada na Linha " << line << "\n";
        } else {
            peek = ' ';
        }
        return new Word(s, LITERAL_STRING);
    }
    
    // Reconhecimento de Números (inteiros e reais)
    if (isdigit((unsigned char)peek)) {
        int v = 0;
        do {
            v = v * 10 + (peek - '0');
            read();
        } while (isdigit((unsigned char)peek));
        
        if (peek != '.') {
            return new NumInt(v);
        }
        
        double x = v;
        double d = 10;
        for (;;) {
            read();
            if (!isdigit((unsigned char)peek)) {
                break;
            }
            x = x + (peek - '0') / d;
            d = d * 10;
        }
        return new NumReal(x);
    }
    
    // Reconhecimento de Identificadores e Palavras-chave
    if (isalpha((unsigned char)peek)) {
        string s = "";
        do {
            s += peek;
            read();
        } while (isalnum((unsigned char)peek) || peek == '_');
        
        if (words.count(s)) {
            return words[s];
        }
        
        Word* w = new Word(s, ID);
        reserve(w);
        return w;
    }
    
    // Caracteres especiais válidos e ignorar BOM
    string valid_chars = "()[],:;+-*%=_";
    if (peek < 0) {
        peek = ' ';
        return scan();
    }
    
    if (valid_chars.find(peek) == string::npos) {
        cerr << "ERRO LEXICO: Simbolo nao reconhecido '" << peek
             << "' na Linha " << line << ", Coluna " << col << "\n";
        peek = ' ';
        return scan();
    }
    
    Token* t = new Token(peek);
    peek = ' ';
    return t;
}

void Lexer::printSymbolTable() const {
    cout << "\n=== Tabela de Simbolos ===\n";
    for (const auto& entry : words) {
        if (entry.second->tag == ID) {
            cout << "ID: " << entry.first << "\n";
        }
    }
    cout << "==========================\n";
}
