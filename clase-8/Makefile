CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g
TARGET := main

# Busca automaticamente TODOS los .cpp del proyecto (main.cpp, Analyzer/*.cpp,
# commands/*.cpp, o cualquier carpeta nueva que agregues despues).
SRCS := $(shell find . -name "*.cpp")

# Convierte cada .cpp en su .o correspondiente, guardandolos en build/
OBJDIR := build
OBJS := $(patsubst ./%,$(OBJDIR)/%,$(SRCS:.cpp=.o))

# Regla por defecto: compilar el ejecutable
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Regla generica: compila cualquier .cpp a su .o dentro de build/
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compila y ejecuta en un solo paso
run: all
	./$(TARGET)

# Limpia archivos generados
clean:
	rm -rf $(OBJDIR) $(TARGET)

# Recompila todo desde cero
rebuild: clean all

.PHONY: all run clean rebuild