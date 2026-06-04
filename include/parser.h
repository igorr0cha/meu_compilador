#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "lexer.h"
#include "token.h"
#include "tags.h"

/**
 * Analisador Sintático (Parser) para a linguagem Portugol.
 * Implementa um parser descendente recursivo (Recursive Descent Parser).
 * 
 * Responsabilidades:
 * - Consumir tokens do lexer.
 * - Validar a estrutura do programa conforme a gramática.
 * - Reportar erros sintáticos com posição (linha, coluna) e contexto.
 * - Implementar cada não-terminal da gramática como uma função.
 * 
 * Gramática (estendida para suportar scripts reais):
 * 
 *  Programa       → algoritmo STR|ID inicio ListaDeclaracoes ListaComandos fim?
 *  ListaDeclaracoes → Declaracao ListaDeclaracoes | ε
 *  Declaracao     → tipo ':' ListaIDs ';'
 *  ListaIDs       → ID ListaIDsRest
 *  ListaIDsRest   → ',' ID ListaIDsRest | ε
 * 
 *  ListaComandos  → Comando ListaComandos | ε
 *  Comando        → Atribuicao | Escrita | Leitura | Se | Enquanto | Para
 * 
 *  Atribuicao     → ID ('[' Expr ']')? '<-' Expr ';'
 *  Escrita        → escreva '(' ListaArgumentos ')' ';'
 *  ListaArgumentos → Argumento (',' Argumento)*
 *  Argumento      → STR | Expr
 *  Leitura        → leia '(' ID ('[' Expr ']')? ')' ';'
 * 
 *  Se             → se Condicao entao ListaComandos SenaoOpcional fim?
 *  SenaoOpcional  → senao ListaComandos | ε
 *  Enquanto       → enquanto Condicao faca ListaComandos fim?
 *  Para           → para ID de Expr ate Expr faca ListaComandos fim?
 * 
 *  Condicao       → Expr OperRel Expr
 *  OperRel        → '=' | '<>' | '>' | '<' | '>=' | '<='
 * 
 *  Expr           → Termo (OpArit Termo)*
 *  Termo          → Fator (OpMult Fator)*
 *  Fator          → Valor | '(' Expr ')'
 * 
 *  Valor          → ID ('[' Expr ']')? | NUM | REAL | STR | VERDADEIRO | FALSO
 */
class Parser {
private:
    Lexer* lexer;
    Token* lookahead;
    
    // ===== Funções Básicas do Parser =====
    
    /**
     * move() - Função de Avanço de Token
     * 
     * Responsabilidade: Solicita o próximo token ao lexer e o armazena em lookahead.
     * 
     * Detalhes:
     * - Chama lexer->scan() para obter o próximo token.
     * - Armazena em lookahead para decisões futuras de parsing.
     * - O token anterior é implicitamente descartado (ou gerenciado pelo lexer).
     * 
     * Utilização: Chamada internamente por match() após validação bem-sucedida.
     */
    void move();
    
    /**
     * match(int tag) - Função de Verificação de Terminal
     * 
     * Responsabilidade: Verifica se o token atual (lookahead) corresponde ao terminal esperado.
     * 
     * Lógica:
     * 1. Se lookahead->tag == tag (o esperado):
     *    - Chama move() para avançar ao próximo token.
     *    - Retorna normalmente (sucesso).
     * 2. Caso contrário:
     *    - Chama error() para reportar erro sintático.
     *    - Interrompe a análise.
     * 
     * Parâmetro: tag - a tag do terminal esperado (ex: ALGORITMO, '(', ';').
     * 
     * Utilização: Usada em toda produção para verificar terminais.
     * Exemplo: match(ALGORITMO) para exigir a palavra-chave "algoritmo".
     */
    void match(int tag);
    
    /**
     * error(string expected) - Função de Reportagem de Erro
     * 
     * Responsabilidade: Gera mensagem de erro clara com contexto e interrompe análise.
     * 
     * Informações exibidas:
     * - Posição do erro (linha, coluna).
     * - Token esperado.
     * - Token encontrado.
     * - Lexema (valor) do token encontrado quando disponível.
     * 
     * Estratégia: Panic mode - para na primeira ocorrência de erro (conforme requisito).
     */
    void error(const std::string& expected);
    
    /**
     * Traduz uma tag para nome legível (para mensagens de erro).
     */
    std::string getTagName(int tag) const;
    
    // ===== Funções de Produção da Gramática =====
    // Cada não-terminal é implementado como uma função privada.
    // Terminais são verificados com match().
    // Alternativas (|) viram if/else.
    // Repetições (*) viram while.
    // Épsilon (ε) é tratado como opcional.
    
    void programa();
    void listaDeclaracoes();
    void declaracao();
    void listaIDs();
    void listaComandos();
    void comando();
    void atribuicao();
    void escrita();
    void listaArgumentos();
    void argumento();
    void leitura();
    void comandoSe();
    void senaoOpcional();
    void comandoEnquanto();
    void comandoPara();
    void condicao();
    void operRel();
    
    // Expressões aritméticas com precedência
    void expr();
    void termo();
    void fator();
    void valor();
    
    int opAritmetico();  // Retorna a tag do operador (+, -, etc.)
    int opMultiplicativo(); // Retorna a tag do operador (*, /, %)

public:
    /**
     * Construtor: associa o parser ao lexer.
     */
    explicit Parser(Lexer* lexer_param);
    
    /**
     * parse() - Função Principal de Análise
     * 
     * Responsabilidade: Inicia a análise sintática do programa.
     * 
     * Etapas:
     * 1. Chama move() para ler o primeiro token.
     * 2. Chama programa() para validar toda a produção raiz.
     * 3. Verifica se chegou ao final do arquivo (sem tokens extra).
     * 
     * Se sucesso: Exibe mensagem de sucesso.
     * Se erro: error() interrompe o programa.
     */
    void parse();
};

#endif // PARSER_H
