CC = gcc
CFLAGS = -Wall -fsanitize=address
# SOURCE = 
SOURCE = stepa.c
EXENAME = stepa.exe
 
all: 
	$(CC) $(CFLAGS) $(SOURCE) -o $(EXENAME)
	./$(EXENAME)