# ARCHITECTURE.md - Guia de Arquitetura e Conceitos

## 📖 Guia de Defesa: O que o Professor Quer Ouvir

O professor vai avaliar sua capacidade de **explicar cada linha principal** do código, especialmente:

1. ✅ A integração entre Parser e Lexer
2. ✅ O papel de `move()`
3. ✅ O papel de `match()`
4. ✅ Como cada produção da gramática foi transformada em função

---

## 🔗 Integração Parser ↔ Lexer

### 1. Acoplamento: Donde vem cada Token?

```
ARQUIVO → Lexer (analisa caracteres) → Token
           ↓
         Token armazenado em `lookahead`
           ↓
        Parser (analisa tokens) → Decisões
```

### 2. Fluxo de Dados Passo a Passo

#### Inicialização (main.cpp)

```cpp
Lexer lexer(filename);      // Cria lexer, abre arquivo
Parser parser(&lexer);      // Cria parser, recebe referência ao lexer
parser.parse();             // Inicia análise
```

**Explicação**:
- O `Parser` recebe um **ponteiro** ao `Lexer`
- Isso permite que o Parser **chame métodos do Lexer** quando precisa
- Exemplo: `Lexer* lexer` → pode chamar `lexer->scan()`

#### Em Parser::parse()

```cpp
void Parser::parse() {
    move();        // Pede 1º token ao lexer via: lookahead = lexer->scan()
    programa();    // Começa análise
}
```

**Explicação**:
- `move()` faz: `lookahead = lexer->scan()`
- `lexer->scan()` retorna o **próximo token do arquivo**
- Armazena em `lookahead` para decisões do parser

#### Em Parser::match()

```cpp
void Parser::match(int tag) {
    if (lookahead != NULL && lookahead->tag == tag) {
        move();  // ← Novo passo: pede próximo token
    } else {
        error(...);
    }
}
```

**Fluxo Completo**:
1. `match(ALGORITMO)` verifica se `lookahead` é "algoritmo"
2. Se SIM: chama `move()` → `lookahead = lexer->scan()` (pega "Teste Simples")
3. Se NÃO: chama `error()` → interrompe

### 3. Separação de Responsabilidades

| Componente | Responsabilidade |
|-----------|------------------|
| **Lexer** | Ler arquivo, reconhecer tokens, retornar via `scan()` |
| **Parser** | Consumir tokens, validar gramática, decidir com base em `lookahead` |
| **main** | Orquestrar: criar lexer → criar parser → executar |

---

## 🎯 move() - Função de Avanço

### Definição Técnica

```cpp
void move() {
    lookahead = lexer->scan();
}
```

### O Que Faz?

1. **Chama o Lexer** para obter o **próximo** token
2. **Armazena** em `lookahead` global
3. **Descarta** o token anterior implicitamente

### Quando é Chamada?

- **1º vez**: Em `parse()` para ler o primeiro token
- **Subsequentemente**: Dentro de `match()` após confirmar um terminal

### Exemplo Visual

```
Estado Inicial:
  Arquivo: algoritmo "Teste" inicio fim
  lookahead: NULL

Após 1º move():
  lookahead: Token(tag=ALGORITMO, lexeme="algoritmo")
             ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑ (1º token)

Após match(ALGORITMO) + move():
  lookahead: Token(tag=LITERAL_STRING, lexeme="Teste")
             ↑↑↑↑↑↑↑↑↑↑ (2º token)

Após match(LITERAL_STRING) + move():
  lookahead: Token(tag=INICIO, lexeme="inicio")
             ↑↑↑↑↑↑ (3º token)
```

### Por Que é Importante?

- **Nexo entre Léxico e Sintático**: Conecta os dois analisadores
- **Lookahead**: Permite ao parser "olhar" 1 token à frente antes de decidir
- **Sincronização**: Mantém parser e lexer sincronizados

---

## ✅ match() - Função de Verificação

### Definição Técnica

```cpp
void match(int tag) {
    if (lookahead != NULL && lookahead->tag == tag) {
        move();  // Terminal esperado encontrado: avança
    } else {
        error(getTagName(tag));  // Terminal incorreto: erro
    }
}
```

### O Que Faz?

1. **Verifica** se `lookahead->tag == tag` (terminal esperado?)
2. **Se SIM**: Chama `move()` para obter próximo token
3. **Se NÃO**: Reporta erro e interrompe

### Exemplo Passo a Passo

```cpp
// Código gramática:  Programa → algoritmo STR inicio ListaComandos fim

void programa() {
    match(ALGORITMO);  // ← Passo 1: Verifica se é "algoritmo"
    match(LITERAL_STRING);  // ← Passo 2: Verifica se é string
    match(INICIO);  // ← Passo 3: Verifica se é "inicio"
    listaComandos();  // ← Processa corpo
    match(FIM);  // ← Passo 4: Verifica se é "fim"
}
```

### Execução com Input "algoritmo "Teste" inicio fim"

```
Chamada 1: match(ALGORITMO)
  lookahead.tag = ALGORITMO ✓
  Ação: move() → lookahead agora é LITERAL_STRING ("Teste")

Chamada 2: match(LITERAL_STRING)
  lookahead.tag = LITERAL_STRING ✓
  Ação: move() → lookahead agora é INICIO

Chamada 3: match(INICIO)
  lookahead.tag = INICIO ✓
  Ação: move() → lookahead agora é primeiro comando

Chamada 4 (em listaComandos): ...processa comandos...

Chamada 5: match(FIM)
  lookahead.tag = FIM ✓
  Ação: move() → lookahead agora é NULL (EOF)
```

### Por Que é Importante?

- **Validação de Gramática**: Garante que tokens chegam na ordem correta
- **Ponte Gramática ↔ C++**: Cada terminal na gramática → 1 `match()`
- **Simpleza**: Código é autorrecuperável (se uma linha está errada, o match falha com mensagem clara)

---

## 🔄 Conversão: Gramática → Código C++

### Exemplo Completo: Atribuição

#### Gramática

```
Atribuicao → ID '<-' Expr ';'
```

**Significado**:
- Começa com um identificador (`ID`)
- Seguido de operador de atribuição (`<-`)
- Uma expressão (`Expr`)
- Termina com ponto-e-vírgula (`;`)

#### Código C++

```cpp
void atribuicao() {
    match(ID);              // Verifica: próximo token é ID?
    match(ATRIBUICAO);      // Verifica: próximo é "<-"?
    expr();                 // Processa expressão (outro não-terminal)
    match(';');             // Verifica: próximo é ";"?
}
```

**Correspondência**:
| Elemento Gramática | Elemento C++ |
|-----------|----------|
| `ID` | `match(ID)` |
| `'<-'` | `match(ATRIBUICAO)` |
| `Expr` | `expr()` (chamada de função) |
| `';'` | `match(';')` |

#### Input de Teste: "idade <- 20 ;"

```cpp
atribuicao() called:
  1. match(ID) → lookahead="idade" ✓, move()
  2. match(ATRIBUICAO) → lookahead="<-" ✓, move()
  3. expr() → processa "20"
  4. match(';') → lookahead=";" ✓, move()
  Retorna com sucesso
```

---

### Exemplo 2: Comando com Alternativas

#### Gramática

```
Comando → Atribuicao | Escrita | Leitura | Se | Enquanto | Para
```

**Significado**: Um comando pode ser qualquer um destes 6 tipos

#### Código C++

```cpp
void comando() {
    if (lookahead == NULL) {
        error("Comando");
    }
    
    switch (lookahead->tag) {
        case ID:       atribuicao(); break;   // Começa com ID → Atribuição
        case ESCREVA:  escrita(); break;      // Começa com "escreva" → Escrita
        case LEIA:     leitura(); break;      // Começa com "leia" → Leitura
        case SE:       comandoSe(); break;    // Começa com "se" → Se
        case ENQUANTO: comandoEnquanto(); break;  // "enquanto" → Enquanto
        case PARA:     comandoPara(); break;  // "para" → Para
        default:       error("Comando (...)");
    }
}
```

**Lógica**: 
- Verifica o `lookahead` atual (qual token estamos vendo?)
- Baseado no `lookahead`, decide qual função chamar
- Cada alternativa tem um **terminal de início** único (FIRST set)

#### Input de Teste: "x <- 5 ;"

```cpp
comando() called:
  lookahead = "x" (ID)
  switch: case ID encontrado
  Chama: atribuicao()
  atribuicao() valida "x <- 5 ;"
```

---

### Exemplo 3: Repetição (ListaComandos)

#### Gramática

```
ListaComandos → Comando ListaComandos | ε
```

**Significado**: Zero ou mais comandos em sequência

#### Código C++

```cpp
void listaComandos() {
    while (lookahead != NULL && (
        lookahead->tag == ID ||        // FIRST(Comando)
        lookahead->tag == ESCREVA ||
        lookahead->tag == LEIA ||
        lookahead->tag == SE ||
        lookahead->tag == ENQUANTO ||
        lookahead->tag == PARA)) {
        
        comando();  // Processa um comando
        // Após comando(), lookahead pode ter mudado
        // Se for outro FIRST(Comando), loop continua
        // Se for FIM ou SENAO, loop para (ε)
    }
}
```

**Lógica**:
- `while` continua enquanto `lookahead` é um `FIRST(Comando)`
- Chama `comando()` para cada token inicial
- Quando encontra token que **não** inicia comando (ex: `FIM`), sai
- Implicitamente processa epsilon

#### Input de Teste: "x <- 5; escreva(x); fim"

```cpp
listaComandos() called:
  Iteração 1:
    lookahead = "x" (ID) ✓ é FIRST(Comando)
    comando() → processa "x <- 5;"
    lookahead agora = "escreva" (ESCREVA)
    
  Iteração 2:
    lookahead = "escreva" ✓ é FIRST(Comando)
    comando() → processa "escreva(x);"
    lookahead agora = "fim" (FIM)
    
  Condição while:
    lookahead = "fim" (FIM) ✗ NÃO é FIRST(Comando)
    Loop sai
    
  Retorna: listaComandos processou 2 comandos
```

---

## 📊 Fluxo Completo: Exemplo "algoritmo "Test" inicio x <- 5; fim"

```
┌─ ARQUIVO ─────────────────────────────────────────────────┐
│ algoritmo "Test" inicio x <- 5; fim                        │
└────────────────────────────────────────────────────────────┘
                    ↓ (scan())
            ┌─ LEXER ────────────┐
            │ Reconhece tokens   │
            └────────────────────┘
                    ↓ (retorna tokens)
┌──────────────────────────────────────────────────────────────┐
│ PARSER - move() e match() em ação                           │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│ parse():                                                     │
│   move() → lookahead = ALGORITMO                             │
│   programa()                                                 │
│                                                              │
│ programa():                                                  │
│   match(ALGORITMO)      ✓ lookahead=ALGORITMO               │
│     move() → lookahead = "Test" (STRING)                     │
│   match(STRING)         ✓ lookahead=STRING                   │
│     move() → lookahead = INICIO                              │
│   match(INICIO)         ✓ lookahead=INICIO                   │
│     move() → lookahead = "x" (ID)                            │
│   listaDeclaracoes()    (vazio)                              │
│   listaComandos()                                            │
│                                                              │
│     comando() → atribuicao()                                 │
│       match(ID)         ✓                                    │
│         move() → lookahead = "<-"                            │
│       match(ATRIBUICAO) ✓                                    │
│         move() → lookahead = "5" (NUM)                       │
│       expr() → ... → lookahead = ";"                         │
│       match(';')        ✓                                    │
│         move() → lookahead = FIM                             │
│                                                              │
│     listaComandos while condition:                           │
│       lookahead = FIM → não é FIRST(Comando) → sai          │
│                                                              │
│   match(FIM)            ✓ lookahead=FIM                      │
│     move() → lookahead = NULL (EOF)                          │
│                                                              │
│ parse() verifica:                                            │
│   lookahead == NULL ✓ (nenhum token extra)                   │
│   Sucesso!                                                   │
│                                                              │
└──────────────────────────────────────────────────────────────┘

                    ↓ (resultado)
            ┌────────────────────┐
            │ SUCESSO!           │
            │ Programa válido    │
            └────────────────────┘
```

---

## 📋 Checklist de Explicação para Defesa

Quando o professor perguntar, tenha em mente:

### ✅ Integração Parser-Lexer
- [ ] "O Parser recebe um ponteiro ao Lexer em seu construtor"
- [ ] "Parser chama `lexer->scan()` via `move()` para obter tokens"
- [ ] "Tokens são armazenados em `lookahead` para decisões"
- [ ] "Esta separação permite analisar léxica independente de sintática"

### ✅ O papel de move()
- [ ] "move() obtém o próximo token do Lexer"
- [ ] "Armazena em `lookahead` para o parser usar"
- [ ] "É chamada após match() confirmar um terminal"
- [ ] "Sincroniza parser e lexer"

### ✅ O papel de match()
- [ ] "match(tag) verifica se lookahead é o terminal esperado"
- [ ] "Se SIM: chama move(), continua análise"
- [ ] "Se NÃO: chama error(), interrompe (panic mode)"
- [ ] "Cada terminal da gramática vira 1 match()"

### ✅ Transformação Gramática → C++
- [ ] "Não-terminal → função com mesmo nome"
- [ ] "Terminal → match(tag)"
- [ ] "Alternativa (|) → switch/if/else"
- [ ] "Repetição (*) → while"
- [ ] "Épsilon (ε) → retorno vazio"

---

## 🎓 Respostas às Perguntas Comuns

### "Por que usar lookahead?"
- Permite decidir qual função chamar **antes** de consumir o token
- Exemplo: Ver "x" (ID) antes de decidir "isto é atribuição"
- Sem lookahead, teríamos que tentar cada alternativa até acertar

### "Qual é a diferença entre Lexer e Parser?"
- **Lexer**: Lê *caracteres* do arquivo, produz *tokens*
- **Parser**: Lê *tokens*, valida *estrutura*
- Exemple: Lexer reconhece "algoritmo" (7 chars) → token ALGORITMO

### "Como tratam erros?"
- **Lexer**: Reporta erro léxico (char inválido), continua (skip)
- **Parser**: Reporta erro sintático com linha/coluna, **interrompe** (panic mode)

### "Por que match() chama move()?"
- Precisa "consumir" o token após validar
- Próxima decisão de parsing precisa do **próximo** token
- match() garante: verifica atual, move para próximo, sincroniza

---

## 🚀 Dicas Finais

1. **Seja concreto**: Use exemplos de input/output
2. **Visualize o fluxo**: Desenhe na lousa o lookahead mudando
3. **Conecte conceitos**: Mostre como `move()` e `match()` trabalham juntos
4. **Cite o código**: "Em main.cpp, criamos `Parser parser(&lexer)`, passando referência"
5. **Seja confiante**: Você entende o projeto, fale com segurança!

---

**Última atualização**: Maio 2026 | Para perguntas, revise este documento!
