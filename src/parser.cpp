#include "../include/parser.h"
#include <cstdlib>
#include <iostream>

using namespace std;

Parser::Parser(Lexer* lexer_param)
    : lexer(lexer_param), lookahead(NULL), sem(nullptr) {
    sem = new SemanticAnalyzer(lexer_param);
}

Parser::~Parser() {
    if (sem) delete sem;
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
    string lexeme_info = (lookahead == NULL) ? "" : " (" + lookahead->lexeme() + ")";

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

int Parser::opMultiplicativo() {
    if (lookahead == NULL) {
        return -1;
    }

    int tag = lookahead->tag;
    if (tag == '*' || tag == '/' || tag == '%') {
        return tag;
    }

    return -1;
}

int Parser::opAritmetico() {
    if (lookahead == NULL) {
        return -1;
    }

    int tag = lookahead->tag;
    if (tag == '+' || tag == '-') {
        return tag;
    }

    return -1;
}

int Parser::valor() {
    if (lookahead == NULL) {
        error("Valor (ID, numero, string, ou logico)");
    }

    switch (lookahead->tag) {
        case ID: {
            Word* w = dynamic_cast<Word*>(lookahead);
            std::string name = w ? w->lexeme() : std::string(1, (char)lookahead->tag);
            match(ID);
            if (lookahead != NULL && lookahead->tag == '[') {
                match('[');
                int idxType = expr();
                match(']');
                // index must be integer
                if (idxType != SemanticAnalyzer::TYPE_INT) {
                    sem->error("Indice de array deve ser inteiro");
                }
                SemanticAnalyzer::Symbol* s = sem->lookup(name);
                if (!s) sem->error("Identificador nao declarado: " + name);
                return s->type; // array element type
            }
            SemanticAnalyzer::Symbol* s = sem->lookup(name);
            if (!s) sem->error("Identificador nao declarado: " + name);
            return s->type;
        }
        case NUM_INT:
            match(NUM_INT);
            return SemanticAnalyzer::TYPE_INT;
        case NUM_REAL:
            match(NUM_REAL);
            return SemanticAnalyzer::TYPE_REAL;
        case LITERAL_STRING:
            match(LITERAL_STRING);
            return SemanticAnalyzer::TYPE_STR;
        case VERDADEIRO:
            match(VERDADEIRO);
            return SemanticAnalyzer::TYPE_BOOL;
        case FALSO:
            match(FALSO);
            return SemanticAnalyzer::TYPE_BOOL;
        default:
            error("Valor (ID, numero, string, ou logico)");
            return SemanticAnalyzer::TYPE_ERROR;
    }
}

int Parser::fator() {
    if (lookahead == NULL) {
        error("Fator (valor ou expressao)");
    }

    if (lookahead->tag == '(') {
        match('(');
        int t = expr();
        match(')');
        return t;
    } else if (lookahead->tag == ID) {
        // valor() handles ID and optional indexing
        return valor();
    } else {
        return valor();
    }
}

int Parser::termo() {
    int t = fator();
    while (opMultiplicativo() != -1) {
        int op = lookahead->tag;
        match(op);
        int right = fator();
        // arithmetic type promotion: int -> real
        if (t == SemanticAnalyzer::TYPE_REAL || right == SemanticAnalyzer::TYPE_REAL) t = SemanticAnalyzer::TYPE_REAL;
        else if (t == SemanticAnalyzer::TYPE_INT && right == SemanticAnalyzer::TYPE_INT) t = SemanticAnalyzer::TYPE_INT;
        else t = SemanticAnalyzer::TYPE_ERROR;
    }
    return t;
}

int Parser::expr() {
    int t = termo();
    while (opAritmetico() != -1) {
        int op = lookahead->tag;
        match(op);
        int right = termo();
        if (t == SemanticAnalyzer::TYPE_REAL || right == SemanticAnalyzer::TYPE_REAL) t = SemanticAnalyzer::TYPE_REAL;
        else if (t == SemanticAnalyzer::TYPE_INT && right == SemanticAnalyzer::TYPE_INT) t = SemanticAnalyzer::TYPE_INT;
        else t = SemanticAnalyzer::TYPE_ERROR;
    }
    return t;
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
    int left = expr();
    operRel();
    int right = expr();
    if (left == SemanticAnalyzer::TYPE_ERROR || right == SemanticAnalyzer::TYPE_ERROR) {
        sem->error("Operacao relacional com tipos invalidos");
    }
    
    // Comparações válidas: entre tipos compatíveis
    bool compatible = (left == right);
    if (left == SemanticAnalyzer::TYPE_INT && right == SemanticAnalyzer::TYPE_REAL) compatible = true;
    if (left == SemanticAnalyzer::TYPE_REAL && right == SemanticAnalyzer::TYPE_INT) compatible = true;
    
    if (!compatible) {
        sem->error("Incompatibilidade de tipos na condicao");
    }
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
    if (lookahead == NULL) error("Atribuicao");
    Word* w = dynamic_cast<Word*>(lookahead);
    std::string name = w ? w->lexeme() : "";
    match(ID);

    bool isArrayAccess = false;
    if (lookahead != NULL && lookahead->tag == '[') {
        match('[');
        int idxType = expr();
        if (idxType != SemanticAnalyzer::TYPE_INT) sem->error("Indice de array deve ser inteiro");
        match(']');
        isArrayAccess = true;
    }

    match(ATRIBUICAO);
    int exprType = expr();
    sem->checkAssignment(name, (SemanticAnalyzer::TypeCode)exprType, isArrayAccess);
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
    if (lookahead == NULL) error("leia");
    Word* w = dynamic_cast<Word*>(lookahead);
    std::string name = w ? w->lexeme() : "";
    match(ID);

    bool isArrayAccess = false;
    if (lookahead != NULL && lookahead->tag == '[') {
        match('[');
        int idxType = expr();
        if (idxType != SemanticAnalyzer::TYPE_INT) sem->error("Indice de array deve ser inteiro");
        match(']');
        isArrayAccess = true;
    }

    // leitura deve referenciar var declarada
    SemanticAnalyzer::Symbol* s = sem->lookup(name);
    if (!s) sem->error("Identificador nao declarado: " + name);

    if (isArrayAccess && !s->isArray) {
        sem->error("Variavel '" + name + "' nao e um array");
    }
    if (!isArrayAccess && s->isArray) {
        sem->error("Variavel '" + name + "' e um array; indice esperado");
    }

    match(')');
    match(';');
}

void Parser::senaoOpcional() {
    if (lookahead != NULL && lookahead->tag == SENAO) {
        match(SENAO);
        sem->enterScope();
        listaComandos();
        sem->exitScope();
    }
}

void Parser::comandoSe() {
    match(SE);
    condicao();
    match(ENTAO);
    sem->enterScope();
    listaComandos();
    sem->exitScope();
    senaoOpcional();

    if (lookahead != NULL && lookahead->tag == FIM) {
        match(FIM);
    }
}

void Parser::comandoEnquanto() {
    match(ENQUANTO);
    condicao();
    match(FACA);
    sem->enterScope();
    listaComandos();
    sem->exitScope();

    if (lookahead != NULL && lookahead->tag == FIM) {
        match(FIM);
    }
}

void Parser::comandoPara() {
    match(PARA);
    if (lookahead == NULL) error("para");
    Word* w = dynamic_cast<Word*>(lookahead);
    std::string iterName = w ? w->lexeme() : "";
    match(ID);
    
    SemanticAnalyzer::Symbol* iterSym = sem->lookup(iterName);
    if (!iterSym) {
        sem->error("Identificador nao declarado: " + iterName);
    }
    
    match(DE);
    int startType = expr();
    match(ATE);
    int endType = expr();
    
    bool startCompatible = (startType == iterSym->type) || (iterSym->type == SemanticAnalyzer::TYPE_REAL && startType == SemanticAnalyzer::TYPE_INT);
    bool endCompatible = (endType == iterSym->type) || (iterSym->type == SemanticAnalyzer::TYPE_REAL && endType == SemanticAnalyzer::TYPE_INT);
    
    if (!startCompatible || !endCompatible) {
        sem->error("Limites do comando 'para' incompativeis com a variavel de controle");
    }
    
    match(FACA);
    sem->enterScope();
    listaComandos();
    sem->exitScope();

    if (lookahead != NULL && lookahead->tag == FIM) {
        match(FIM);
    }
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

void Parser::listaIDs(SemanticAnalyzer::TypeCode declaredType) {
    if (lookahead == NULL) error("Lista de IDs");

    if (lookahead->tag != ID) error("ID");
    Word* w = dynamic_cast<Word*>(lookahead);
    std::string name = w ? w->lexeme() : "";
    match(ID);

    bool isArray = false;
    if (lookahead != NULL && lookahead->tag == '[') {
        match('[');
        match(NUM_INT);
        match(']');
        isArray = true;
    }
    sem->declare(name, declaredType, isArray, lexer->getLine(), lexer->getCol());

    while (lookahead != NULL && lookahead->tag == ',') {
        match(',');
        if (lookahead == NULL || lookahead->tag != ID) error("ID");
        Word* w2 = dynamic_cast<Word*>(lookahead);
        std::string name2 = w2 ? w2->lexeme() : "";
        match(ID);
        bool isArray2 = false;
        if (lookahead != NULL && lookahead->tag == '[') {
            match('[');
            match(NUM_INT);
            match(']');
            isArray2 = true;
        }
        sem->declare(name2, declaredType, isArray2, lexer->getLine(), lexer->getCol());
    }
}

void Parser::declaracao() {
    if (lookahead == NULL || (
        lookahead->tag != INTEIRO &&
        lookahead->tag != REAL &&
        lookahead->tag != CARACTERE &&
        lookahead->tag != LOGICO)) {
        error("Tipo (inteiro, real, caractere, ou logico)");
    }

    int declaredTag = lookahead->tag;
    match(lookahead->tag);
    match(':');
    listaIDs(sem->tokenTagToType(declaredTag));
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

    if (lookahead != NULL && lookahead->tag == LITERAL_STRING) {
        match(LITERAL_STRING);
    } else {
        match(ID);
    }

    listaDeclaracoes();
    match(INICIO);
    listaComandos();

    if (lookahead != NULL && lookahead->tag == FIM) {
        match(FIM);
    } else if (lookahead == NULL) {
        // EOF válido: o fechamento do programa já foi absorvido por um bloco aninhado.
    } else {
        error("fim");
    }
}

void Parser::parse() {
    move();
    programa();

    if (lookahead != NULL) {
        error("Fim de Arquivo (nenhum token deve vir apos 'fim')");
    }
}
