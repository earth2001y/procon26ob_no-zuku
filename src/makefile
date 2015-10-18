CXX=g++
LD=g++
CXXFLAGS=-O3 -ftree-vectorize -msse4.2 #-ftree-vectorizer-verbose=2
#CXXFLAGS=-O0 -ggdb
LDFLAGS=
TARGET=a.out

SRC = main.cpp game.cpp piece.cpp state.cpp
OBJ = $(SRC:%.cpp=%.o)
ASMS = $(SRC:%.cpp=%.s)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

.cpp.s:
	$(CXX) $(CXXFLAGS) -S $<

all: $(TARGET)

$(TARGET): $(OBJ)
	$(LD) -o $(TARGET) $(LDFLAGS) $(OBJ)

asm: $(ASMS)

clean:
	rm -f $(OBJ) $(ASMS)

