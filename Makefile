CC = gcc
CFLAGS = -c -g -Wall
OBJECTS = helper_func.o shell.o execute_cmd.o
EXECUTABLE = pkwshell

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE)

execute_cmd.o: execute_cmd.c execute_cmd.h shell.h helper_func.h
	$(CC) $(CFLAGS) execute_cmd.c

helper_func.o: helper_func.c helper_func.h shell.h
	$(CC) $(CFLAGS) helper_func.c

shell.o: shell.c shell.h helper_func.h
	$(CC) $(CFLAGS) shell.c

clean:
	rm $(OBJECTS) $(EXECUTABLE)
