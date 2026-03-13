#include <iostream>
#include <string>
#include "tags.h"
#include "lexer.h"

using namespace std;

int main() {
    string filename;
    
    // O programa agora pede o nome do arquivo diretamente na tela
    cout << "### Analisador Lexico Portugol ###\n" << endl;
    cout << "Digite o nome do arquivo de texto (ex: teste.txt): ";
    cin >> filename;
    cout << "\n----------------------------------------\n" << endl;

    Lexer lexer(filename);
    
    while(true) {
        Token* t = lexer.scan();
        
        if (t == NULL) break; // Fim do arquivo

        if(t->tag == NUM_INT) {
            NumInt* n = static_cast<NumInt*>(t);
            cout << "<NUM_INT, " << n->value << ">\n";
        } 
        else if(t->tag == NUM_REAL) {
            NumReal* n = static_cast<NumReal*>(t);
            cout << "<NUM_REAL, " << n->value << ">\n";
        }
        else if(t->tag == LITERAL_STRING) {
            Word* w = static_cast<Word*>(t);
            cout << "<STRING, \"" << w->lexeme << "\">\n";
        }
        else if(t->tag == ID) {
            Word* w = static_cast<Word*>(t);
            cout << "<ID, \"" << w->lexeme << "\">\n";
        }
        else if(t->tag >= ALGORITMO && t->tag <= MAIOR_IGUAL) {
            Word* w = static_cast<Word*>(t);
            cout << "<RESERVADA/OP_COMP, \"" << w->lexeme << "\">\n";
        }
        else {
            // Operadores simples e pontuação
            cout << "<DELIM/OP, '" << (char)t->tag << "'>\n";
        }
    }

    // Exibe a tabela de símbolos no final da execução, como pedido
    lexer.printSymbolTable();

    cout << "\nAnalise concluida. Pressione Enter para sair..." << endl;
    cin.ignore();
    cin.get();

    return 0;
}
