SRC = hybrid.cpp

# The c++ compiler we are using
CXX = c++

# The c++ flags we use for compilation
CXXFLAGS = -Wall -Wextra -g -std=c++14 -O3 -fsanitize=address  

# eliminate default suffixes
.SUFFIXES:
SUFFIXES =

# Just consider our own suffixes
.SUFFIXES: .cpp .x

# Serial compilation
serial: $(SRC)
	@echo "Making serial version"
	$(CXX) $^ -o $@.x $(CXXFLAGS) 
	
.PHONY: serial

# Parallel compilation
parallel: $(SRC) 
	@echo "Making parallel version"
	$(CXX) $^ -o $@.x -fopenmp $(CXXFLAGS)
	
.PHONY: parallel

# Formatting
format: $(SRC)
	@clang-format -i $^ -verbose || echo "Please install clang-format to run this command"

.PHONY: format

# Cleaning
clean:
	rm -f $(EXE) *~

.PHONY: clean

# Headers
format: dataset.hpp
format: info.hpp
format: tree.hpp
