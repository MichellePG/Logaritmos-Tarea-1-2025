CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
EXEC = tarea1

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f src/*.o $(EXEC)

run:
	./tarea1 datos/datos_sample.txt
