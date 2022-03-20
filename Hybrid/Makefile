# The executable
EXE = tree.x

# The compiler
CXX = mpic++

# Flags for the compiler
CXXFLAGS = -std=c++14 -I include
FLAG =

# Conditional flag
ifdef parallel
	FLAG += -fopenmp
endif

# Targets
all: $(EXE)

# Rules to make the object file
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)$(FLAG)

# Rules to make the executable
$(EXE): tree.o src/info.o
	$(CXX) $^ -o $(EXE)$(FLAG)
	@rm tree.o

# Dependencies
tree.o: src/info.o
src/info.o: include/info.hpp

clean:
	rm src/*.o $(EXE)

.PHONY: clean all format
