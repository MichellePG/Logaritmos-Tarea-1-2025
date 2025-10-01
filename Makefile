CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -Iinclude

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
EXEC = tarea1

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f src/*.o $(EXEC)

run-experimento: $(EXEC)
	./$(EXEC) --experimento --datos datos/datos.bin > resultados.csv
	@echo "Resultados guardados en resultados.csv"
