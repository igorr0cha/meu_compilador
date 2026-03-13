#include <iostream>
#include "tags.h"
#include "lexer.h"

using namespace std;

int main() {
    Lexer lexer;
    cout << "### Analisador Lexico ###" << endl;
    cout << "Digite o codigo (ex: inteiro x = 10 + verdadeiro) e tecle Enter:" << endl;

    while(true) {
        Token* t = lexer.scan();
        
        if (t == NULL) break;

        if(t->tag == NUM) {
            Num* n = static_cast<Num*>(t);
            cout << "<NUM, " << n->value << "> ";
        } 
        else if(t->tag == ID) {
            Word* w = static_cast<Word*>(t);
            cout << "<ID, \"" << w->lexeme << "\"> ";
        }
        // Verifica se a tag está no intervalo das palavras reservadas
        else if(t->tag >= VERDADEIRO && t->tag <= IMPRIMIR) {
            Word* w = static_cast<Word*>(t);
            cout << "<RESERVADA, \"" << w->lexeme << "\"> ";
        }
        else {
            cout << "<OP, '" << (char)t->tag << "'> ";
        }
        
        if (t->tag == ';') cout << endl;
    }

    return 0;
}
