# The executable
EXE = tree.x

# The compiler
CXX = mpic++

# Flags for the compiler
CXXFLAGS = -fopenmp -std=c++14 -I include  

# Targets
all: $(EXE)

# Rules to make the object file
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Rules to make the executable
$(EXE): tree.o src/communication.o src/info.o
	$(CXX) $^ -o $(EXE)

src/communication.o: include/communication.hpp
src/info.o: include/info.hpp

clean:
	rm src/*.o *.o $(EXE)

.PHONY: clean all format
