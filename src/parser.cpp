#include "../include/parser.h"
#include <iostream>
#include <cstdlib>

using namespace std;

Parser::Parser(Lexer* lexer_param)
    : lexer(lexer_param), lookahead(NULL) {
}

void Parser::move() {
    lookahead = lexer->scan();
}

string Parser::getTagName(int tag) const {
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
        case INTEIRO: return "inteiro";
        case REAL: return "real";
        case CARACTERE: return "caractere";
        case LOGICO: return "logico";
        case END_OF_FILE: return "EOF";
        default: return string(1, (char)tag);
    }
}

void Parser::error(const string& expected) {
    cout << "\n[ERRO SINTATICO] Linha " << lexer->getLine()
         << ", Coluna " << lexer->getCol() << ":\n";
    
    string found = (lookahead == NULL) ? "Fim de Arquivo" : getTagName(lookahead->tag);
    string lexeme_info = "";
    
    if (lookahead != NULL) {
        lexeme_info = " (" + lookahead->lexeme() + ")";
    }
    
    cout << "  Esperado: '" << expected << "'\n";
    cout << "  Encontrado: '" << found << "'" << lexeme_info << "\n";
    cout << "\nAnalise interrompida devido a erro sintatico.\n";
    
    exit(1);
}

void Parser::match(int tag) {
    if (lookahead != NULL && lookahead->tag == tag) {
        move();
    } else {
        error(getTagName(tag));
    }
}

// ===== Funções de Produção da Gramática =====

void Parser::valor() {
    if (lookahead == NULL) {
        error("Valor (ID, numero, string, ou logico)");
    }
    switch (lookahead->tag) {
        case ID:
            match(ID);
            break;
        case NUM_INT:
            match(NUM_INT);
            break;
        case NUM_REAL:
            match(NUM_REAL);
            break;
        case LITERAL_STRING:
            match(LITERAL_STRING);
            break;
        case VERDADEIRO:
            match(VERDADEIRO);
            break;
        case FALSO:
            match(FALSO);
            break;
        default:
            error("Valor (ID, numero, string, ou logico)");
    }
}

int Parser::opMultiplicativo() {
    if (lookahead == NULL) return -1;
    
    int tag = lookahead->tag;
    if (tag == '*' || tag == '/' || tag == '%') {
        return tag;
    }
    return -1;
}

int Parser::opAritmetico() {
    if (lookahead == NULL) return -1;
    
    int tag = lookahead->tag;
    if (tag == '+' || tag == '-') {
        return tag;
    }
    return -1;
}

void Parser::fator() {
    if (lookahead == NULL) {
        error("Fator (valor ou expressao)");
    }
    
    if (lookahead->tag == '(') {
        match('(');
        expr();
        match(')');
    } else if (lookahead->tag == ID) {
        match(ID);
        // Suporte a acesso a vetores: ID[expr]
        if (lookahead != NULL && lookahead->tag == '[') {
            match('[');
            expr();
            match(']');
        }
    } else {
        valor();
    }
}

void Parser::termo() {
    fator();
    while (opMultiplicativo() != -1) {
        int op = lookahead->tag;
        match(op);
        fator();
    }
}

void Parser::expr() {
    termo();
    while (opAritmetico() != -1) {
        int op = lookahead->tag;
        match(op);
        termo();
    }
}

void Parser::operRel() {
    if (lookahead == NULL) {
        error("Operador Relacional");
    }
    
    switch (lookahead->tag) {
        case '=':
            match('=');
            break;
        case DIFERENTE:
            match(DIFERENTE);
            break;
        case '>':
            match('>');
            break;
        case '<':
            match('<');
            break;
        case MAIOR_IGUAL:
            match(MAIOR_IGUAL);
            break;
        case MENOR_IGUAL:
            match(MENOR_IGUAL);
            break;
        default:
            error("Operador Relacional (=, <>, >, <, >=, <=)");
    }
}

void Parser::condicao() {
    expr();
    operRel();
    expr();
}

void Parser::argumento() {
    if (lookahead != NULL && lookahead->tag == LITERAL_STRING) {
        match(LITERAL_STRING);
    } else {
        expr();
    }
}

void Parser::listaArgumentos() {
    if (lookahead == NULL) {
        error("Argumentos");
    }
    
    argumento();
    while (lookahead != NULL && lookahead->tag == ',') {
        match(',');
        argumento();
    }
}

void Parser::atribuicao() {
    match(ID);
    
    // Suporte a acesso a vetores no LHS
    if (lookahead != NULL && lookahead->tag == '[') {
        match('[');
        expr();
        match(']');
    }
    
    match(ATRIBUICAO);
    expr();
    match(';');
}

void Parser::escrita() {
    match(ESCREVA);
    match('(');
    listaArgumentos();
    match(')');
    match(';');
}

void Parser::leitura() {
    match(LEIA);
    match('(');
    match(ID);
    
    // Suporte a acesso a vetores
    if (lookahead != NULL && lookahead->tag == '[') {
        match('[');
        expr();
        match(']');
    }
    
    match(')');
    match(';');
}

void Parser::senaoOpcional() {
    if (lookahead != NULL && lookahead->tag == SENAO) {
        match(SENAO);
        listaComandos();
    }
}

void Parser::comandoSe() {
    match(SE);
    condicao();
    match(ENTAO);
    listaComandos();
    senaoOpcional();
    // Nota: Os scripts reais não usam 'fim' para cada bloco 'se'.
    // O 'fim' final do programa encerra todos os blocos.
    // Portanto, não exigimos 'fim' aqui.
}

void Parser::comandoEnquanto() {
    match(ENQUANTO);
    condicao();
    match(FACA);
    listaComandos();
    // Nota: Os scripts reais não usam 'fim' para cada bloco 'enquanto'.
    // O 'fim' final do programa encerra todos os blocos.
}

void Parser::comandoPara() {
    match(PARA);
    match(ID);
    match(DE);
    expr();
    match(ATE);
    expr();
    match(FACA);
    listaComandos();
    // Nota: Os scripts reais não usam 'fim' para cada bloco 'para'.
    // O 'fim' final do programa encerra todos os blocos.
}

void Parser::comando() {
    if (lookahead == NULL) {
        error("Comando");
    }
    
    switch (lookahead->tag) {
        case ID:
            atribuicao();
            break;
        case ESCREVA:
            escrita();
            break;
        case LEIA:
            leitura();
            break;
        case SE:
            comandoSe();
            break;
        case ENQUANTO:
            comandoEnquanto();
            break;
        case PARA:
            comandoPara();
            break;
        default:
            error("Comando (atribuicao, escreva, leia, se, enquanto, ou para)");
    }
}

void Parser::listaComandos() {
    while (lookahead != NULL && (
        lookahead->tag == ID ||
        lookahead->tag == ESCREVA ||
        lookahead->tag == LEIA ||
        lookahead->tag == SE ||
        lookahead->tag == ENQUANTO ||
        lookahead->tag == PARA)) {
        comando();
    }
}

void Parser::listaIDs() {
    match(ID);
    
    // Suporte a dimensão de vetor: ID[NUM]
    if (lookahead != NULL && lookahead->tag == '[') {
        match('[');
        match(NUM_INT);
        match(']');
    }
    
    while (lookahead != NULL && lookahead->tag == ',') {
        match(',');
        match(ID);
        
        if (lookahead != NULL && lookahead->tag == '[') {
            match('[');
            match(NUM_INT);
            match(']');
        }
    }
}

void Parser::declaracao() {
    // tipo : listaIDs ;
    // Tipo já foi verificado pelo chamador
    if (lookahead == NULL || (
        lookahead->tag != INTEIRO &&
        lookahead->tag != REAL &&
        lookahead->tag != CARACTERE &&
        lookahead->tag != LOGICO)) {
        error("Tipo (inteiro, real, caractere, ou logico)");
    }
    
    match(lookahead->tag); // Consome o tipo
    match(':');
    listaIDs();
    match(';');
}

void Parser::listaDeclaracoes() {
    while (lookahead != NULL && (
        lookahead->tag == INTEIRO ||
        lookahead->tag == REAL ||
        lookahead->tag == CARACTERE ||
        lookahead->tag == LOGICO)) {
        declaracao();
    }
}

void Parser::programa() {
    match(ALGORITMO);
    
    // Nome do programa (STR ou ID)
    if (lookahead != NULL && lookahead->tag == LITERAL_STRING) {
        match(LITERAL_STRING);
    } else {
        match(ID);
    }
    
    // Declarações opcionais
    listaDeclaracoes();
    
    match(INICIO);
    listaComandos();
    match(FIM);
}

void Parser::parse() {
    move(); // Pega o primeiro token
    programa(); // Analisa o programa
    
    // Verifica se há tokens extras após "fim"
    if (lookahead != NULL) {
        error("Fim de Arquivo (nenhum token deve vir apos 'fim')");
    }
}
