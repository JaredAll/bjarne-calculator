CXX = clang++
CXX_FLAGS = -Werror -pedantic-errors -O0 -Wno-c++98-compat-pedantic -Wno-padded -std=c++11

all: calculator

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $<

calculator: calculator.o
	$(CXX) $(LINKER_FLAGS) -o calculator $(OBJ_FILES) calculator.o

run: calculator
	./calculator

.PHONY : clean

clean: 
	-rm *.o

.DELETE_ON_ERROR: 
