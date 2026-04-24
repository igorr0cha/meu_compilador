#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <cstdlib>
#include "lexer.h"
#include "tags.h"

using namespace std;

// Classe responsável pela Análise Sintática (Parser) via Descida Recursiva (Recursive Descent)
class Parser {
private:
    Lexer* lexer;     // Ponteiro para o Lexer já desenvolvido
    Token* lookahead; // Token atual sendo avaliado na gramática

    // --- FUNÇÕES BÁSICAS DO PARSER ---

    // Função de avanço de token (move):
    // Solicita o próximo token ao analisador léxico.
    void move() {
        lookahead = lexer->scan();
    }

    // Auxiliar para mensagens de erro: Traduz a Tag (int) para um nome descritivo legível.
    string getTagName(int tag) {
        switch (tag) {
            case NUM_INT: return "NUM_INT";
            case NUM_REAL: return "NUM_REAL";
            case ID: return "ID";
            case LITERAL_STRING: return "STR";
            case ALGORITMO: return "algoritmo";
            case INICIO: return "inicio";
            case FIM: return "fim";
            case SE: return "se";
            case ENTAO: return "entao";
            case SENAO: return "senao";
            case ENQUANTO: return "enquanto";
            case FACA: return "faca";
            case PARA: return "para";
            case DE: return "de";
            case ATE: return "ate";
            case ESCREVA: return "escreva";
            case LEIA: return "leia";
            case ATRIBUICAO: return "<-";
            case DIFERENTE: return "<>";
            case MENOR_IGUAL: return "<=";
            case MAIOR_IGUAL: return ">=";
            case VERDADEIRO: return "verdadeiro";
            case FALSO: return "falso";
            case END_OF_FILE: return "EOF";
            default: return string(1, (char)tag);
        }
    }

    // Reporta erro sintático indicando linha/coluna (utilizado para panic mode/encerramento limpo)
    void error(string expected) {
        cout << "\n[ERRO SINTATICO] Linha " << lexer->getLine() 
             << ", Coluna " << lexer->getCol() << ":\n";
        
        string found = (lookahead == NULL) ? "Fim de Arquivo (EOF)" : getTagName(lookahead->tag);
        
        cout << " -> Esperado '" << expected << "', mas encontrado '" << found << "'." << endl;
        cout << "\nA compilacao foi interrompida devido a erros." << endl;
        
        exit(1); // Interrupção limpa na primeira ocorrência de erro (conforme permitido pelo enunciado)
    }

    // Função de verificação (match):
    // Verifica se o token atual (lookahead) corresponde ao símbolo terminal exigido.
    // Em caso afirmativo, avança (chama move()). Em caso negativo, dispara erro.
    void match(int tag) {
        if (lookahead != NULL && lookahead->tag == tag) {
            move();
        } else {
            error(getTagName(tag));
        }
    }

    // --- REGRAS DE PRODUÇÃO DA GRAMÁTICA ---
    // Cada não-terminal da gramática foi transformado rigorosamente em um método.

    // Valor -> ID | NUM | REAL | STR | TRUE | FALSE
    void valor() {
        if (lookahead == NULL) { error("Valor"); return; }
        switch (lookahead->tag) {
            case ID:             match(ID); break;
            case NUM_INT:        match(NUM_INT); break;
            case NUM_REAL:       match(NUM_REAL); break;
            case LITERAL_STRING: match(LITERAL_STRING); break;
            case VERDADEIRO:     match(VERDADEIRO); break;
            case FALSO:          match(FALSO); break;
            default:
                error("Valor (ID, numero, string, ou logico)");
        }
    }

    // Argumento -> STR | Valor
    // Nota C++: Como STR (LITERAL_STRING) já está contido na regra de "Valor" acima, 
    // a invocação a Argumento pode ser resolvida delegando a verificação à função "valor()".
    void argumento() {
        valor();
    }

    // OperRel -> = | <> | > | < | >= | <=
    void operRel() {
        if (lookahead == NULL) { error("Operador Relacional"); return; }
        switch (lookahead->tag) {
            case '=':         match('='); break;
            case DIFERENTE:   match(DIFERENTE); break;
            case '>':         match('>'); break;
            case '<':         match('<'); break;
            case MAIOR_IGUAL: match(MAIOR_IGUAL); break;
            case MENOR_IGUAL: match(MENOR_IGUAL); break;
            default:
                error("Operador Relacional (=, <>, >, <, >=, <=)");
        }
    }

    // Condicao -> Valor OperRel Valor
    void condicao() {
        valor();
        operRel();
        valor();
    }

    // Atribuicao -> ID <- Valor ;
    void atribuicao() {
        match(ID);
        match(ATRIBUICAO); // Operador <-
        valor();
        match(';');
    }

    // Escrita -> escreva ( Argumento ) ;
    void escrita() {
        match(ESCREVA);
        match('(');
        argumento(); 
        match(')');
        match(';');
    }

    // Leitura -> leia ( ID ) ;
    void leitura() {
        match(LEIA);
        match('(');
        match(ID);
        match(')');
        match(';');
    }

    // SenaoOpcional -> senao ListaComandos | epsilon
    void senaoOpcional() {
        // Diferencia da produção épsilon: se o token for 'senao', processa as ramificações
        if (lookahead != NULL && lookahead->tag == SENAO) {
            match(SENAO);
            listaComandos();
        }
        // Caso contrário, é o epsilon: a recursão simplesmente sobe limpa sem erro.
    }

    // Se -> se Condicao entao ListaComandos SenaoOpcional fim
    void comandoSe() {
        match(SE);
        condicao();
        match(ENTAO);
        listaComandos();
        senaoOpcional();
        match(FIM);
    }

    // Enquanto -> enquanto Condicao faca ListaComandos fim
    void comandoEnquanto() {
        match(ENQUANTO);
        condicao();
        match(FACA);
        listaComandos();
        match(FIM);
    }

    // Para -> para ID de Valor ate Valor faca ListaComandos fim
    void comandoPara() {
        match(PARA);
        match(ID);
        match(DE);
        valor();
        match(ATE);
        valor();
        match(FACA);
        listaComandos();
        match(FIM);
    }

    // Comando -> Atribuicao | Escrita | Leitura | Se | Enquanto | Para
    void comando() {
        if (lookahead == NULL) { error("Comando"); return; }
        switch (lookahead->tag) {
            case ID:       atribuicao(); break;
            case ESCREVA:  escrita(); break;
            case LEIA:     leitura(); break;
            case SE:       comandoSe(); break;
            case ENQUANTO: comandoEnquanto(); break;
            case PARA:     comandoPara(); break;
            default:
                // Força um erro se nenhum dos starts válidos do FIRST(Comando) for encontrado
                error("Comando (atribuicao, escreva, leia, se, enquanto, ou para)");
        }
    }

    // ListaComandos -> Comando ListaComandos | epsilon
    void listaComandos() {
        // Loop que implementa a recursão/Lista de 'Comando' avaliando o grupo FIRST de comandos.
        while (lookahead != NULL && (
               lookahead->tag == ID ||
               lookahead->tag == ESCREVA ||
               lookahead->tag == LEIA ||
               lookahead->tag == SE ||
               lookahead->tag == ENQUANTO ||
               lookahead->tag == PARA)) {
            comando();
        }
        // Épsilon transition: se não encontrar nenhum token de comando, sai graciosamente.
    }

    // Programa -> algoritmo STR inicio ListaComandos fim
    void programa() {
        match(ALGORITMO);
        
        // Muitos códigos podem empregar ID como identificador do algoritmo ao invés de string.
        // O código cobre caso seja um LITERAL_STRING ou um ID comum.
        if (lookahead != NULL && lookahead->tag == LITERAL_STRING) {
            match(LITERAL_STRING);
        } else {
            match(ID);
        }

        match(INICIO);
        
        listaComandos();
        
        match(FIM);
    }

public:
    // Construtor: Exige o Lexer já instanciado no sistema.
    Parser(Lexer* lexer_param) {
        lexer = lexer_param;
        lookahead = NULL;
    }

    // Inicia a validação da gramática
    void parse() {
        move();        // Pega o primeiro token
        programa();    // Inicia pela raiz da Árvore de Derivação (Programa)
        
        // Após ler 'fim' do programa, assegura que não existe lixo (não detectado como erro ainda).
        if (lookahead != NULL) {
            error("Fim de Arquivo (Nada mais é esperado apoś a keyword 'fim' do programa principal)");
        }
    }
};

#endif