#include <iostream>
#include <string>
#include "include/lexer.h"
#include "include/parser.h"

using namespace std;

int main(int argc, char* argv[]) {
    string filename;
    
    cout << "============================================\n";
    cout << "   Compilador Portugol - Analisador Sintatico\n";
    cout << "============================================\n\n";
    
    // Obtém o nome do arquivo
    if (argc > 1) {
        filename = argv[1];
        cout << "Arquivo: " << filename << "\n\n";
    } else {
        cout << "Uso: ./compilador <arquivo.txt>\n";
        cout << "Exemplo: ./compilador scripts/script_simples.txt\n\n";
        cout << "Digite o caminho do arquivo: ";
        cin >> filename;
    }
    
    cout << "Iniciando analise...\n";
    cout << "-------------------------------------------\n\n";
    
    try {
        // Cria o lexer
        Lexer lexer(filename);
        
        // Cria o parser e realiza análise sintática
        Parser parser(&lexer);
        parser.parse();
        
        // Sucesso
        cout << "\n===========================================\n";
        cout << "SUCESSO!\n";
        cout << "O programa esta sintaticamente valido.\n";
        cout << "===========================================\n";
        
        // Exibe tabela de simbolos
        lexer.printSymbolTable();
        
    } catch (const exception& e) {
        cerr << "\nERRO: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
