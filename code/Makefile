# The executable
EXE = kdtree.x

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
$(EXE): kdtree.o src/info.o
	$(CXX) $^ -o $(EXE)$(FLAG)
	@rm kdtree.o

# Dependencies
tree.o: src/info.o
src/info.o: include/info.hpp

clean:
	rm src/*.o $(EXE)

format: include/dataset.hpp include/info.hpp include/node.hpp include/communication.hpp kdtree.cpp

	@clang-format -i $^ -verbose || echo "Please install clang-format to run this command"

.PHONY: clean all format

