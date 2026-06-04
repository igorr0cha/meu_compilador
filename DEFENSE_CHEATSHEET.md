# DEFENSE_CHEATSHEET.md - Guia Rápido para a Defesa

## ⏱️ Prepare-se em 5 Minutos!

Use este guia durante a defesa como referência rápida.

---

## 📍 1. Integração Parser-Lexer (primeira pergunta!)

**Pergunta Professor**: "Como o Parser se integra ao Lexer?"

**Sua Resposta**:
> "O Parser recebe uma referência ao Lexer no construtor. 
> Quando Parser precisa do próximo token, chama `lexer->scan()` 
> através da função `move()`. Tokens são armazenados em `lookahead` 
> para o Parser tomar decisões. É análise descendente recursiva."

**Apontador no Código**:
- `include/parser.h`: Vê `Lexer* lexer` (linha ~15)
- `src/parser.cpp`: Vê `move()` que chama `lookahead = lexer->scan()` (linha ~52)

---

## 🔄 2. Função move() (segunda pergunta!)

**Pergunta Professor**: "Qual é o papel da função move()?"

**Sua Resposta**:
> "move() obtém o próximo token do Lexer. Código: 
> `lookahead = lexer->scan();` 
> É chamada no início de parse() para ler primeiro token, 
> e recursivamente dentro de match() para avançar após confirmar cada terminal."

**Código para Mostrar**:
```cpp
// Em parser.cpp, linha ~52
void Parser::move() {
    lookahead = lexer->scan();
}
```

---

## ✅ 3. Função match() (terceira pergunta!)

**Pergunta Professor**: "Como funciona match()?"

**Sua Resposta**:
> "match(tag) verifica se o token atual (lookahead) corresponde 
> ao terminal esperado. Se SIM: chama move() para obter próximo. 
> Se NÃO: reporta erro com linha, coluna e contexto. 
> Cada terminal na gramática vira um match()."

**Código para Mostrar**:
```cpp
// Em parser.cpp, linha ~67
void Parser::match(int tag) {
    if (lookahead != NULL && lookahead->tag == tag) {
        move();  // ✓ Terminal correto
    } else {
        error(getTagName(tag));  // ✗ Terminal incorreto
    }
}
```

**Exemplo Prático**:
- Input: `algoritmo "Teste" inicio fim`
- `programa()` chama `match(ALGORITMO)` 
- lookahead = ALGORITMO ✓
- move() → lookahead = STRING ("Teste")

---

## 🔗 4. Transformação Gramática → C++ (se pedir mais!)

**Pergunta Professor**: "Como você transforma gramática em C++?"

**Padrões (5 principais)**:

### Padrão 1: Não-Terminal → Função
```
Gramática:   Fator → Valor | '(' Expr ')'
C++:         void fator() { ... }
```

### Padrão 2: Terminal → match()
```
Gramática:   Atribuicao → ID '<-' Expr ';'
C++:         match(ID);
             match(ATRIBUICAO);
             expr();
             match(';');
```

### Padrão 3: Alternativa (|) → switch
```
Gramática:   Comando → Atribuicao | Escrita | Leitura | ...
C++:         switch(lookahead->tag) {
               case ID: atribuicao(); break;
               case ESCREVA: escrita(); break;
               ...
             }
```

### Padrão 4: Repetição (*) → while
```
Gramática:   ListaComandos → Comando*
C++:         while(lookahead->tag == ID || 
                   lookahead->tag == ESCREVA || ...) {
               comando();
             }
```

### Padrão 5: Epsilon (ε) → retorno vazio
```
Gramática:   SenaoOpcional → senao ListaComandos | ε
C++:         if (lookahead->tag == SENAO) {
               match(SENAO);
               listaComandos();
             }
             // Caso contrário: apenas retorna
```

---

## 🧪 5. Testes: Como Demonstrar

**Pergunta Professor**: "Execute um teste para mim"

**Sua Ação**:
```bash
cd ~/Documentos/meu_compilador
./compilador scripts/script_simples.txt

# Esperado: [SUCESSO!]
```

**Se Pedir Teste Mais Complexo**:
```bash
./compilador scripts/script_dificil.txt

# Prova:
# - Vetores
# - Expressões complexas com precedência
# - Aninhamento de estruturas
# - Múltiplas declarações
```

---

## 🎯 6. Estrutura SOLID (se pedir design)

**Pergunta Professor**: "Mostre que você seguiu princípios SOLID"

**Sua Resposta com Evidências**:

| Princípio | Evidência | Arquivo |
|-----------|-----------|---------|
| **SRP** | Lexer faz análise léxica, Parser faz análise sintática | `src/lexer.cpp` vs `src/parser.cpp` |
| **OCP** | Token é base, NumInt/NumReal/Word são subclasses | `include/token.h` |
| **LSP** | Subclasses substituem Token sem problema | `include/token.h` |
| **ISP** | Parser usa apenas 3 métodos do Lexer: scan(), getLine(), getCol() | `include/lexer.h` |
| **DIP** | Parser não conhece internals do Lexer | `Parser` depende de `Lexer*` abstrato |

---

## 🔥 7. Perguntas Difíceis (prepare-se!)

### "Por que usar lookahead?"
> "Permite decidir qual função chamar ANTES de consumir o token. 
> Vejo 'x' (ID), sei que vai ser atribuição sem tentar outras opções."

### "Como você trata erros?"
> "Panic mode: primeira erro interrompe. Mensagens mostram 
> linha, coluna (do Lexer), token esperado vs. encontrado, e lexema. 
> Exemplo: '[ERRO SINTATICO] Linha 5, Coluna 10: 
> Esperado ← encontrado fim'"

### "Por que match() chama move()?"
> "Precisa CONSUMIR o token após validar. Próxima decisão precisa 
> do PRÓXIMO token. match() garante: verifica, consome, sincroniza."

### "O que faz se encontrar token inválido?"
> "Se lookahead não é token esperado, error() interrompe com contexto completo."

### "Qual a diferença Lexer e Parser?"
> "Lexer lê CARACTERES, produz TOKENS (exemplo: 'algoritmo' → ALGORITMO). 
> Parser lê TOKENS, valida ESTRUTURA."

---

## 📋 8. Checklist Defesa (Dia D!)

- [ ] Compilar: `make clean && make` (3 segundos)
- [ ] Teste 1: `./compilador scripts/script_simples.txt` → SUCESSO
- [ ] Teste 2: `./compilador scripts/script_medio.txt` → SUCESSO
- [ ] Teste 3: `./compilador scripts/script_dificil.txt` → SUCESSO
- [ ] Explicar move(): "Obtém próximo token do Lexer"
- [ ] Explicar match(): "Verifica terminal, avança se OK"
- [ ] Mostrar integração: "Parser recebe Lexer*, chama lexer->scan()"
- [ ] SOLID: "5 princípios aplicados, ver arquivo"
- [ ] Gramática: "5 padrões de transformação"

---

## 🎤 9. Seu Discurso Inicial (1 minuto)

> "Meu compilador implementa análise léxica e sintática para Portugol 
> usando análise descendente recursiva em C++11. 
> 
> Arquitetura: 
> - Lexer (scan tokens do arquivo)
> - Parser (valida gramática) integrado ao Lexer via move() e match()
> - Separação SOLID: cada responsabilidade seu arquivo
> 
> Funcionalidades:
> - 6 estruturas (if/while/for/leia/escreva/atribuição)
> - Vetores com índices
> - Expressões com precedência
> - Erros com linha, coluna, contexto
> 
> Testes: todos 3 scripts passando (simples, médio, difícil).
> 
> Vou demonstrar agora..."

---

## 💪 10. Dicas Finais

✅ **Seja concreto**: Use exemplos específicos do código
✅ **Use terminologia**: "Análise descendente", "token", "lookahead", "match"
✅ **Mostre confiança**: Você ENTENDE o projeto
✅ **Ouça a pergunta**: Responda exatamente o que foi perguntado
✅ **Use o código**: "Em parser.cpp linha X, vê-se..."
✅ **Prepare exemplos**: Saiba o que cada script testa
✅ **Não decore**: Entenda e explique com suas palavras

---

## 🏆 Você está pronto!

Seu projeto é:
- ✅ Completo (todos requisitos)
- ✅ Testado (3/3 scripts passando)
- ✅ Bem arquitetado (SOLID)
- ✅ Documentado (README + ARCHITECTURE)
- ✅ Profissional (código limpo, comentários claros)

**Boa defesa!** 🎓

---

*Última atualização: Maio 2026*
*Print this page ou guarde no celular para consultar rápido!*
