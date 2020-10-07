
CC := g++
CFLAGS := -Wall -Wextra -Werror -Wno-unused -std=gnu++17 -g -c
LDFLAGS := 
ifeq ($(TEST),1)
CFLAGS += -DTEST -DCOLORS
endif
	
all: main

main: main.o line_sort.o
	 $(CC) $(LDFLAGS) line_sort.o main.o -o main

main.o: main.cpp
	 $(CC) $(CFLAGS) main.cpp

line_sort.o: line_sort.cpp line_sort.hpp
	 $(CC) $(CFLAGS) line_sort.cpp

clean:
	rm -f *.o main
	
	
