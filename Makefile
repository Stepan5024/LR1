CC = gcc
CFLAGS = -Wall -fsanitize=address
# SOURCE = 
SOURCE = stepa2.c
EXENAME = stepa2.exe
 
all: 
	$(CC) $(CFLAGS) $(SOURCE) -o $(EXENAME)
	./$(EXENAME)