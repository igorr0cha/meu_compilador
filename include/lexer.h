#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <map>
#include <fstream>
#include "token.h"
#include "tags.h"

/**
 * Analisador Léxico (Lexer) para a linguagem Portugol.
 * 
 * Responsabilidades:
 * - Ler caracteres do arquivo fonte.
 * - Reconhecer tokens (palavras-chave, identificadores, números, strings).
 * - Manter posição no arquivo (linha e coluna) para mensagens de erro.
 * - Retornar tokens ao analisador sintático via scan().
 * 
 * Single Responsibility Principle: Responsável apenas pela análise léxica.
 * Dependency Inversion: O parser depende da interface do lexer, não de detalhes internos.
 */
class Lexer {
private:
    char peek;
    std::map<std::string, Word*> words;
    std::ifstream file;
    int line;
    int col;
    
    /**
     * Registra uma palavra-chave na tabela de símbolos.
     */
    void reserve(Word* w);
    
    /**
     * Lê o próximo caractere do arquivo e atualiza posição.
     */
    void read();
    
    /**
     * Lê um caractere específico.
     * Retorna true se o caractere atual é o esperado.
     */
    bool readch(char c);

public:
    /**
     * Construtor: abre o arquivo e inicializa a tabela de palavras-chave.
     */
    explicit Lexer(const std::string& filename);
    
    /**
     * Destrutor: fecha o arquivo e libera memória.
     */
    ~Lexer();
    
    /**
     * Retorna o número da linha atual (1-indexed).
     * Usado para mensagens de erro.
     */
    int getLine() const { return line; }
    
    /**
     * Retorna o número da coluna atual (1-indexed).
     * Usado para mensagens de erro.
     */
    int getCol() const { return col; }
    
    /**
     * Analisa o próximo token do arquivo.
     * 
     * Retorna:
     * - Ponteiro para o Token reconhecido.
     * - NULL se chegou ao final do arquivo.
     * 
     * Esta é a interface principal do lexer para o parser.
     */
    Token* scan();
    
    /**
     * Exibe a tabela de símbolos (identificadores) encontrados.
     */
    void printSymbolTable() const;
};

#endif // LEXER_H
