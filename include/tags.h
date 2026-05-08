#ifndef TAGS_H
#define TAGS_H

/**
 * Enumeração de tags de tokens utilizados pelo analisador léxico.
 * Cada tag representa uma categoria de token reconhecida pelo lexer.
 * 
 * Tags < 256 são caracteres ASCII (ex: '(', ')', ';', etc.).
 * Tags >= 256 são palavras-chave, identificadores e literais.
 */
enum Tag {
    // Literais e Identificadores
    NUM_INT = 256,
    NUM_REAL = 257,
    ID = 258,
    LITERAL_STRING = 259,
    
    // Palavras-chave: Estrutura e Controle de Fluxo
    ALGORITMO = 260, 
    INICIO = 261, 
    FIM = 262,
    SE = 263, 
    ENTAO = 264, 
    SENAO = 265,
    ENQUANTO = 266, 
    FACA = 267,
    PARA = 268, 
    DE = 269, 
    ATE = 270,
    
    // Tipos de Variáveis (suporte para declarações)
    INTEIRO = 271, 
    REAL = 272, 
    CARACTERE = 273, 
    LOGICO = 274,
    
    // Entrada e Saída
    ESCREVA = 275, 
    LEIA = 276,
    
    // Operadores Compostos e Relacionais
    ATRIBUICAO = 277,  // <-
    DIFERENTE = 278,   // <>
    MENOR_IGUAL = 279, // <=
    MAIOR_IGUAL = 280, // >=
    
    // Valores Lógicos
    VERDADEIRO = 281,
    FALSO = 282,
    
    // Fim de Arquivo
    END_OF_FILE = 283
};

#endif // TAGS_H
