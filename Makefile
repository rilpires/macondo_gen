# Basic makefile

CXX := g++
# Parameterize the compiler flags if we want to debug
CXXFLAGS := -std=c++14 -I include # -Wall 
CUSTOM_CXXFLAGS :=

# Diretórios
SRC_DIR := src
OBJ_DIR := bin
BIN_DIR := bin

# Arquivos de origem
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
# Gera os nomes dos arquivos de objeto
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
# Nome do executável
TARGET := $(BIN_DIR)/macondogen

# Regra padrão
all: $(TARGET)

# Regra para o executável
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(CUSTOM_CXXFLAGS) $^ -o $@

# Regra para os arquivos de objeto
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CUSTOM_CXXFLAGS) -c $< -o $@

# Limpa os arquivos gerados
clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*

# Faz a limpeza antes de reconstruir
rebuild: clean all

# Define as regras que não representam arquivos
.PHONY: all clean rebuild
