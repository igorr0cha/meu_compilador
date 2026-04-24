#include <iostream>
#include <string>
#include "tags.h"
#include "lexer.h"
#include "parser.h"

using namespace std;

int main(int argc, char* argv[]) {
    string filename;
    
    cout << "### Analisador Sintatico Portugol ###\n" << endl;

    if (argc > 1) {
        filename = argv[1];
        cout << "Lendo do arquivo: " << filename << endl;
    } else {
        cout << "Dica: Em compiladores profissionais, passe o arquivo como argumento!" << endl;
        cout << "Exemplo: ./compilador lexer/teste.txt\n" << endl;
        cout << "Digite o caminho do arquivo de texto (ex: lexer/teste.txt): ";
        cin >> filename;
    }
    cout << "\n----------------------------------------\n" << endl;

    Lexer lexer(filename);
    Parser parser(&lexer);
    
    cout << "Iniciando a Analise Sintatica...\n" << endl;
    parser.parse();

    cout << "\n=============================================" << endl;
    cout << "Analise Sintatica concluida com SUCESSO!" << endl;
    cout << "O programa obedece integralmente a gramatica." << endl;
    cout << "=============================================\n" << endl;

    lexer.printSymbolTable();

    cout << "\nPressione Enter para sair..." << endl;
    cin.ignore();
    cin.get();

    return 0;
}
