TARGET= a.out
OBJECTS=main.o include/BAL.o 
TOBJECTS=test.o include/Bal.o 
CFLAGS= -Wall 

default : main

all : main

run: main
	echo "Starting Program Execution...\r\n\r\n"
	./$(TARGET)
	echo "\r\n\r\nEnd Test Program Execution...\r\n\r\n"
	make clean
	
clean :
	rm -f *.o
	rm -f $(TARGET)
	rm -f ./include/*.o
	
%.o : %.c
	gcc -c $< -o $@

main : $(OBJECTS)
	gcc $(CFLAGS) $(OBJECTS) -o $(TARGET)

test : $(TOBJECTS)
	gcc $(CFLAGS) $(TOBJECTS) -o $(TARGET)
	echo "Starting Test Program Execution...\r\n\r\n"
	./$(TARGET)
	echo "\r\n\r\nEnd Test Program Execution..."
	make clean
