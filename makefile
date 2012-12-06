
CXX=g++
#CXXFLAGS=-Ofast -DNDEBUG -fomit-frame-pointer -march=native -msse4
CXXFLAGS=-Ofast -DNDEBUG -fomit-frame-pointer -finline-functions -march=native -msse4.2

all:
	$(CXX) $(CXXFLAGS) -o sudoku0 sudoku0.cpp
	$(CXX) $(CXXFLAGS) -o sudoku1 sudoku1.cpp
	$(CXX) $(CXXFLAGS) -o sudoku2 sudoku2.cpp
	$(CXX) $(CXXFLAGS) -o sudoku3 sudoku3.cpp

test:
	./sudoku0 q_kawai.txt 20000000 | head -1
	./sudoku1 q_kawai.txt 20000000 | head -1
	./sudoku2 q_kawai.txt 20000000 | head -1
	./sudoku3 q_kawai.txt 20000000 | head -1

