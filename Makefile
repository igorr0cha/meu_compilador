# Makefile para Compilador Portugol

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -I.
LDFLAGS =

# Diretórios
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = .

# Arquivos fonte
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/lexer.cpp $(SRC_DIR)/parser.cpp $(SRC_DIR)/semantic.cpp
OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/lexer.o $(BUILD_DIR)/parser.o $(BUILD_DIR)/semantic.o
EXECUTABLE = $(BIN_DIR)/compilador

# Alvo padrão
all: $(EXECUTABLE)

# Compilação do executável
$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(LDFLAGS) -o $@ $^
	@echo "Build completo: $(EXECUTABLE)"

# Compilação de objetos
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE)
	@echo "Limpeza completa"

# Execução com script de teste
test-simples: $(EXECUTABLE)
	./$(EXECUTABLE) scripts/script_simples.txt

test-medio: $(EXECUTABLE)
	./$(EXECUTABLE) scripts/script_medio.txt

test-dificil: $(EXECUTABLE)
	./$(EXECUTABLE) scripts/script_dificil.txt

test-all: test-simples test-medio test-dificil

# Phony targets
.PHONY: all clean test-simples test-medio test-dificil test-all
