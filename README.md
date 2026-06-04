# Compilador Portugol - Analisador Sintático

Um compilador modular e bem-arquiteturado para a linguagem Portugol, desenvolvido em C++ seguindo os princípios SOLID.

## 📋 Descrição

Este projeto implementa um **analisador léxico (Lexer)** e um **analisador sintático (Parser)** para a linguagem Portugol. O parser utiliza a técnica de **análise descendente recursiva (Recursive Descent Parsing)** para validar a estrutura dos programas Portugol.

### Estrutura de Diretórios

```
compilador/
├── include/          # Arquivos header (.h)
│   ├── tags.h        # Enumeração de tokens
│   ├── token.h       # Classes de tokens
│   ├── lexer.h       # Interface do lexer
│   └── parser.h      # Interface do parser
├── src/              # Implementação (.cpp)
│   ├── main.cpp      # Programa principal
│   ├── lexer.cpp     # Implementação do lexer
│   └── parser.cpp    # Implementação do parser
├── scripts/          # Scripts de teste
│   ├── script_simples.txt
│   ├── script_medio.txt
│   ├── script_dificil.txt
│   └── teste.txt
├── build/            # Diretório de saída (gerado)
├── Makefile          # Build automation
└── README.md         # Este arquivo
```

## 🏗️ Arquitetura

### Princípios SOLID Aplicados

1. **Single Responsibility Principle (SRP)**
   - `Lexer`: Responsável apenas por análise léxica
   - `Parser`: Responsável apenas por análise sintática
   - `Token`: Representa dados de tokens

2. **Open/Closed Principle (OCP)**
   - Classes aberta para extensão (herança de `Token`)
   - Fechada para modificação (interfaces bem definidas)

3. **Liskov Substitution Principle (LSP)**
   - Subclasses de `Token` (`NumInt`, `NumReal`, `Word`) substituem a classe base

4. **Interface Segregation Principle (ISP)**
   - Métodos públicos bem definidos e específicos

5. **Dependency Inversion Principle (DIP)**
   - `Parser` depende da abstração `Lexer`, não de detalhes internos

## 🔧 Compilação e Execução

### Usando Makefile

```bash
# Compilar o projeto
make

# Executar testes
make test-simples    # Script simples
make test-medio      # Script médio
make test-dificil    # Script difícil
make test-all        # Todos os testes

# Limpar arquivos gerados
make clean
```

### Executar com um Script

```bash
./compilador <caminho_do_arquivo>

# Exemplo:
./compilador scripts/script_simples.txt
```

## 📚 Gramática da Linguagem

### Produção

```
Programa       → algoritmo (STR|ID) ListaDeclaracoes inicio ListaComandos fim?

ListaDeclaracoes → Declaracao* | ε
Declaracao     → tipo ':' ListaIDs ';'

ListaComandos  → Comando* | ε
Comando        → Atribuicao | Escrita | Leitura | Se | Enquanto | Para

Atribuicao     → ID ('[' Expr ']')? '<-' Expr ';'
Escrita        → escreva '(' ListaArgumentos ')' ';'
Leitura        → leia '(' ID ('[' Expr ']')? ')' ';'

Se             → se Condicao entao ListaComandos SenaoOpcional fim?
SenaoOpcional  → senao ListaComandos | ε
Enquanto       → enquanto Condicao faca ListaComandos fim?
Para           → para ID de Expr ate Expr faca ListaComandos fim?

Condicao       → Expr OperRel Expr
OperRel        → '=' | '<>' | '>' | '<' | '>=' | '<='

Expr           → Termo (OpArit Termo)*
Termo          → Fator (OpMult Fator)*
Fator          → Valor | '(' Expr ')'
Valor          → ID ('[' Expr ']')? | NUM | REAL | STR | VERDADEIRO | FALSO
```

## 🔑 Conceitos Principais

### move() - Função de Avanço de Token

```cpp
void move() {
    lookahead = lexer->scan();
}
```

**Responsabilidade**: Obter o próximo token do lexer.

**Fluxo**:
1. Chama `lexer->scan()` para obter novo token do arquivo
2. Armazena em `lookahead` global para o parser consultar
3. Token anterior é descartado automaticamente

**Quando é chamado**: 
- No início de `parse()` para ler primeiro token
- Dentro de `match()` após confirmar um terminal
- Recursivamente até consumir todos os tokens

### match(int tag) - Função de Verificação de Terminal

```cpp
void match(int tag) {
    if (lookahead != NULL && lookahead->tag == tag) {
        move();  // Terminal correto: avança
    } else {
        error(getTagName(tag));  // Terminal incorreto: erro
    }
}
```

**Responsabilidade**: Validar se o token atual corresponde ao esperado pela gramática.

**Lógica Fundamental**:
1. Compara `lookahead->tag` com `tag` esperado
2. **Se iguais** (terminal correto):
   - Chama `move()` para obter próximo token
   - Análise continua normalmente
3. **Se diferentes** (terminal incorreto):
   - Chama `error()` exibindo esperado vs. encontrado
   - Interrompe análise (Panic Mode)

**Exemplo Prático**:
```cpp
void programa() {
    match(ALGORITMO);  // Exige token "algoritmo"
    // Se o próximo token for "algoritmo": move() → continua
    // Se não for: error() → interrompe
    
    match(INICIO);     // Exige token "inicio"
    listaComandos();   // Processa corpo do programa
    if (lookahead != NULL && lookahead->tag == FIM) {
        match(FIM);
    } else if (lookahead == NULL) {
        // EOF válido quando um bloco interno já absorveu o fechamento.
    } else {
        error("fim");
    }
}
```

### Acoplamento Parser ↔ Lexer

**Dependência de Interface**:
- `Parser` depende apenas dos métodos públicos de `Lexer`
- Interface bem definida em `include/lexer.h`

**Métodos Utilizados**:
```cpp
Token* scan()        // Retorna próximo token
int getLine()        // Posição para erros
int getCol()         // Posição para erros
```

**Benefícios**:
- Baixo acoplamento: `Lexer` pode ser trocado sem quebrar `Parser`
- Fácil teste: Pode-se mockar o `Lexer`
- Separação de responsabilidades clara

## 💡 Transformação: Gramática → C++

### Padrão 1: Não-Terminal → Função

```
Gramática:   Valor → ID | NUM | REAL | STR | TRUE | FALSE
             (cada não-terminal vira função)

C++:         void valor() {
                 if (lookahead->tag == ID)
                     match(ID);
                 else if (lookahead->tag == NUM_INT)
                     match(NUM_INT);
                 // ... casos adicionais
             }
```

### Padrão 2: Terminal → match()

```
Gramática:   Atribuicao → ID '<-' Valor ';'
             (terminais são verificados com match)

C++:         void atribuicao() {
                 match(ID);              // Verifica ID
                 match(ATRIBUICAO);      // Verifica '<-'
                 valor();                // Processa não-terminal
                 match(';');             // Verifica ';'
             }
```

### Padrão 3: Alternativa (|) → if/else

```
Gramática:   Comando → Atribuicao | Escrita | Leitura | Se | Enquanto | Para

C++:         void comando() {
                 if (lookahead->tag == ID)
                     atribuicao();
                 else if (lookahead->tag == ESCREVA)
                     escrita();
                 // ... demais alternativas
             }
```

### Padrão 4: Repetição (*) → while

```
Gramática:   ListaComandos → Comando Comando ... | ε
             (zero ou mais repetições)

C++:         void listaComandos() {
                 while (lookahead != NULL &&
                        (lookahead->tag == ID ||
                         lookahead->tag == ESCREVA ||
                         // ... FIRST(Comando)
                        )) {
                     comando();
                 }
             }
```

### Padrão 5: Epsilon (ε) → Retorno Vazio

```
Gramática:   SenaoOpcional → senao ListaComandos | ε
             (opcional)

C++:         void senaoOpcional() {
                 if (lookahead != NULL && lookahead->tag == SENAO) {
                     match(SENAO);
                     listaComandos();
                 }
                 // Caso contrário: implicitamente ε
                 // Função retorna sem fazer nada
             }
```

## 📝 Exemplo de Execução

### Entrada: script_simples.txt

```portugol
algoritmo "Teste Simples"

inteiro: idade;
real: nota;

inicio
idade <- 20;
nota <- 8.5;
escreva("Idade: ", idade);
leia(idade);

fim
```

### Saída

```
============================================
   Compilador Portugol - Analisador Sintatico
============================================

Arquivo: scripts/script_simples.txt

Iniciando analise...
-------------------------------------------


===========================================
SUCESSO!
O programa esta sintaticamente valido.
===========================================

=== Tabela de Simbolos ===
ID: idade
ID: nota
==========================
```

## 🧪 Teste dos 3 Scripts

```bash
$ make test-all

./compilador scripts/script_simples.txt
[✓] SUCESSO!

./compilador scripts/script_medio.txt
[✓] SUCESSO!

./compilador scripts/script_dificil.txt
[✓] SUCESSO!
```

## 🐛 Tratamento de Erros

### Estratégia: Panic Mode

Interrompe na primeira ocorrência de erro:

```
[ERRO SINTATICO] Linha 5, Coluna 10:
  Esperado: '<-'
  Encontrado: 'fim' (fim)

Analise interrompida devido a erro sintatico.
```

### Informações Exibidas

- **Linha e Coluna**: Posição exata do erro (do lexer)
- **Token Esperado**: O que a gramática exigia
- **Token Encontrado**: Qual token foi encontrado
- **Lexema**: Valor do token (para ID, NUM, STR, etc.)

## 📋 Checklist de Requisitos

- ✅ Integração com o Léxico (Parser consome tokens via `scan()`)
- ✅ Token Atual (lookahead mantido)
- ✅ Função move() (avança para próximo token)
- ✅ Função match() (verifica terminais)
- ✅ Reconhecimento de Estruturas (se, enquanto, para, leia, escreva)
- ✅ Gerenciamento de Erros (mensagens clara com linha/coluna)
- ✅ Estrutura do Parser (cada não-terminal é função)
- ✅ Passa nos 3 scripts (simples, médio, difícil)
- ✅ Organização Modular (SOLID principles)
- ✅ Documentação Completa

## 🚀 Extensões Futuras

1. Construção de Árvore Sintática Abstrata (AST)
2. Análise Semântica (tipos, escopo)
3. Otimizações de Código
4. Geração de Bytecode/Assembly
5. Recuperação de Erros com sincronização

---

**Desenvolvido**: Maio 2026 | **Linguagem**: C++11 | **Padrão**: SOLID
