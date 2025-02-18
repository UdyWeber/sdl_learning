OBJS = key_press.cpp
CC = clang++
COMPILER_FLAGS = -w
LINKER_FLAGS = -lSDL2 -lSDL2_image
TARGET = key_press

all: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LINKER_FLAGS) $(COMPILER_FLAGS)

clean:
	rm -rf $(TARGET)
