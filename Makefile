OBJS = sand.cpp 
CC = clang++
COMPILER_FLAGS = -w
LINKER_FLAGS = -lSDL2 -lSDL2_image
TARGET = program
STD = -std=c++20

all: $(OBJS)
	$(CC) $(STD) -o $(TARGET) $(OBJS) $(LINKER_FLAGS) $(COMPILER_FLAGS)

clean:
	rm -rf $(TARGET)
