TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
		./$(TARGET) -f ./mynewdb.db -n
		./$(TARGET) -f ./mynewdb.db -a "Timmy H., 1 Fountainblue Dr., 120"
		./$(TARGET) -f ./mynewdb.db -a "Zack, 2 3rd Street N., 17"
		./$(TARGET) -f ./mynewdb.db -a "Jimmy, 3 47th Street S., 18"
		./$(TARGET) /bin/dbview -f ./mynewdb.db -r "Timmy H."
		./$(TARGET) /bin/dbview -f ./mynewdb.db -l 
		./$(TARGET) -f ./mynewdb.db -a "Jimmy, 4 47th Street S., 19"

default: $(TARGET)

clean:
		rm -f obj/*.o
		rm -f bin/*
		rm -f *.db

$(TARGET): $(OBJ)
		gcc -o $@ $?

obj/%.o : src/%.c
		gcc -c $< -o $@ -Iinclude